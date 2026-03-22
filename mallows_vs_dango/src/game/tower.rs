use bevy::prelude::*;

use super::enemy::{Dango, DangoState, DangoStats};

/// Marker component: this entity is a Tower.
#[derive(Component)]
pub struct Tower;

/// Tower state machine, mirroring the C++ Tower::State.
#[derive(Component, Default, PartialEq, Clone, Copy, Debug)]
pub enum TowerState {
    #[default]
    Idle,
    Aware,
    Attacking,
    Reloading,
    Placing,
}

/// Combat stats for a tower instance.
#[derive(Component)]
pub struct TowerStats {
    pub tower_type: String,
    pub level: usize,
    pub damage: f32,
    pub range: f32,
    pub attack_speed: f32,
    pub cost: f32,
    pub xp: f32,
    pub xp_to_next: f32,
}

/// Timer tracking attack cooldown.
#[derive(Component)]
pub struct AttackTimer {
    pub timer: Timer,
}

/// The tower's current target enemy entity.
#[derive(Component)]
pub struct TowerTarget {
    pub target: Option<Entity>,
}

/// Grid position of a placed tower.
#[derive(Component)]
pub struct GridPosition {
    pub col: usize,
    pub row: usize,
}

/// System: towers find targets (closest enemy in range that's closest to path end).
pub fn tower_targeting_system(
    mut tower_query: Query<
        (&Transform, &TowerStats, &mut TowerTarget, &mut TowerState),
        With<Tower>,
    >,
    enemy_query: Query<
        (Entity, &Transform, &DangoStats, &DangoState, &super::enemy::PathFollower),
        With<Dango>,
    >,
) {
    for (tower_tf, tower_stats, mut tower_target, mut state) in &mut tower_query {
        if *state == TowerState::Placing || *state == TowerState::Attacking {
            continue;
        }

        let tower_pos = tower_tf.translation.truncate();
        let range_sq = tower_stats.range * tower_stats.range;

        // Find the best target: in range, alive, furthest along path
        let mut best: Option<(Entity, usize)> = None; // (entity, waypoint_progress)

        for (entity, enemy_tf, _stats, enemy_state, follower) in &enemy_query {
            if *enemy_state == DangoState::Dead || *enemy_state == DangoState::Dying {
                continue;
            }

            let enemy_pos = enemy_tf.translation.truncate();
            let dist_sq = tower_pos.distance_squared(enemy_pos);

            if dist_sq <= range_sq {
                let progress = follower.current_waypoint;
                if best.map_or(true, |(_, bp)| progress > bp) {
                    best = Some((entity, progress));
                }
            }
        }

        match best {
            Some((entity, _)) => {
                tower_target.target = Some(entity);
                if *state == TowerState::Idle {
                    *state = TowerState::Aware;
                }
            }
            None => {
                tower_target.target = None;
                if *state == TowerState::Aware {
                    *state = TowerState::Idle;
                }
            }
        }
    }
}

/// System: towers attack their targets on cooldown.
pub fn tower_attack_system(
    time: Res<Time>,
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    mut tower_query: Query<
        (
            &Transform,
            &TowerStats,
            &TowerTarget,
            &mut TowerState,
            &mut AttackTimer,
        ),
        With<Tower>,
    >,
    mut enemy_query: Query<(&Transform, &mut DangoStats), With<Dango>>,
) {
    for (tower_tf, tower_stats, tower_target, mut state, mut atk_timer) in &mut tower_query {
        atk_timer.timer.tick(time.delta());

        match *state {
            TowerState::Aware => {
                if atk_timer.timer.finished() {
                    if let Some(target_entity) = tower_target.target {
                        if let Ok((enemy_tf, mut enemy_stats)) =
                            enemy_query.get_mut(target_entity)
                        {
                            // Queue prospective damage
                            enemy_stats.prospective_damage += tower_stats.damage;
                            enemy_stats.hp -= tower_stats.damage;

                            // Spawn projectile
                            let tower_pos = tower_tf.translation.truncate();
                            let enemy_pos = enemy_tf.translation.truncate();

                            commands.spawn((
                                Sprite::from_image(
                                    asset_server
                                        .load("res/turret/attacks/water_ball.png"),
                                ),
                                Transform::from_translation(tower_pos.extend(10.0)),
                                super::projectile::Projectile {
                                    target: target_entity,
                                    speed: 600.0,
                                    damage: tower_stats.damage,
                                    target_pos: enemy_pos,
                                },
                                super::projectile::ProjectileCleanup,
                            ));

                            *state = TowerState::Reloading;
                            atk_timer
                                .timer
                                .set_duration(std::time::Duration::from_secs_f32(
                                    tower_stats.attack_speed,
                                ));
                            atk_timer.timer.reset();
                        }
                    }
                }
            }
            TowerState::Reloading => {
                if atk_timer.timer.finished() {
                    *state = if tower_target.target.is_some() {
                        TowerState::Aware
                    } else {
                        TowerState::Idle
                    };
                }
            }
            _ => {}
        }
    }
}
