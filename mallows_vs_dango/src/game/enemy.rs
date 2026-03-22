use bevy::prelude::*;

/// Marker component: this entity is a Dango enemy.
#[derive(Component)]
pub struct Dango;

/// Enemy state machine, mirroring the C++ Dango::STATE.
#[derive(Component, Default, PartialEq, Clone, Copy, Debug)]
pub enum DangoState {
    #[default]
    Idle,
    Moving,
    Attacking,
    Reloading,
    Dying,
    Dead,
}

/// Combat stats for an enemy instance.
#[derive(Component)]
pub struct DangoStats {
    pub max_hp: f32,
    pub hp: f32,
    /// Prospective (queued) damage not yet applied.
    pub prospective_damage: f32,
    pub speed: f32,
    pub attack_damage: f32,
    pub attack_reload: f32,
    pub xp_reward: f32,
    pub sugar_reward: u32,
    /// Dango type key (e.g. "dangosimple").
    pub dango_type: String,
    /// Difficulty level index.
    pub level: usize,
}

/// Path-following component.
#[derive(Component)]
pub struct PathFollower {
    /// Waypoints in world space.
    pub waypoints: Vec<Vec2>,
    /// Index of the current target waypoint.
    pub current_waypoint: usize,
    /// Normalized direction toward current waypoint.
    pub direction: Vec2,
}

impl PathFollower {
    pub fn new(waypoints: Vec<Vec2>) -> Self {
        let direction = if waypoints.len() > 1 {
            (waypoints[1] - waypoints[0]).normalize_or_zero()
        } else {
            Vec2::ZERO
        };
        Self {
            waypoints,
            current_waypoint: 1,
            direction,
        }
    }

    /// Returns true if the enemy has reached the end of the path.
    pub fn is_done(&self) -> bool {
        self.current_waypoint >= self.waypoints.len()
    }
}

/// Health bar UI for an enemy, rendered as a child sprite.
#[derive(Component)]
pub struct HealthBar;

/// System: move enemies along their paths.
pub fn enemy_movement_system(
    time: Res<Time>,
    grid: Option<Res<crate::game::grid::Grid>>,
    mut query: Query<
        (&mut Transform, &mut PathFollower, &DangoStats, &DangoState),
        With<Dango>,
    >,
) {
    let Some(grid) = grid else { return };
    let dt = time.delta_secs();

    for (mut transform, mut follower, stats, state) in &mut query {
        if *state != DangoState::Moving || follower.is_done() {
            continue;
        }

        let speed = stats.speed * grid.cell_size * dt;
        let current_pos = transform.translation.truncate();
        let target = follower.waypoints[follower.current_waypoint];
        let to_target = target - current_pos;
        let dist = to_target.length();

        if dist <= speed {
            // Reached waypoint
            transform.translation.x = target.x;
            transform.translation.y = target.y;
            follower.current_waypoint += 1;

            if !follower.is_done() {
                let next = follower.waypoints[follower.current_waypoint];
                follower.direction = (next - target).normalize_or_zero();
            }
        } else {
            let movement = follower.direction * speed;
            transform.translation.x += movement.x;
            transform.translation.y += movement.y;
        }
    }
}

/// System: remove dead enemies and enemies that reached the end.
pub fn enemy_cleanup_system(
    mut commands: Commands,
    query: Query<(Entity, &DangoState, &PathFollower), With<Dango>>,
    mut lives: Option<ResMut<crate::game::game_scene::GameLives>>,
) {
    for (entity, state, follower) in &query {
        if *state == DangoState::Dead {
            commands.entity(entity).despawn_recursive();
        } else if follower.is_done() && *state == DangoState::Moving {
            // Enemy reached the end — lose a life
            if let Some(ref mut lives) = lives {
                lives.current = lives.current.saturating_sub(1);
            }
            commands.entity(entity).despawn_recursive();
        }
    }
}

/// System: handle enemy taking damage and dying.
pub fn enemy_damage_system(
    mut query: Query<(&mut DangoStats, &mut DangoState), With<Dango>>,
) {
    for (stats, mut state) in &mut query {
        if *state == DangoState::Dying || *state == DangoState::Dead {
            continue;
        }
        if stats.hp <= 0.0 {
            *state = DangoState::Dead;
        }
    }
}
