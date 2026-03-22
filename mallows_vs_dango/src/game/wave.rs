use bevy::prelude::*;

use crate::config::DangoConfigs;
use super::enemy::{Dango, DangoState, DangoStats, PathFollower};
use super::level_data::LevelData;

/// Resource: manages wave spawning for the current level.
#[derive(Resource)]
pub struct WaveManager {
    /// Current wave index (0-based).
    pub current_wave: usize,
    /// Current spawn step within the wave.
    pub current_step: usize,
    /// Accumulated time since last spawn.
    pub timer: f64,
    /// Whether wave spawning is active (player pressed "start wave").
    pub active: bool,
    /// Total enemies spawned across all waves.
    pub total_spawned: usize,
    /// Total waves in this level.
    pub total_waves: usize,
}

impl Default for WaveManager {
    fn default() -> Self {
        Self {
            current_wave: 0,
            current_step: 0,
            timer: 0.0,
            active: false,
            total_spawned: 0,
            total_waves: 0,
        }
    }
}

impl WaveManager {
    pub fn new(total_waves: usize) -> Self {
        Self {
            total_waves,
            ..Default::default()
        }
    }

    pub fn is_wave_done(&self, level_data: &LevelData) -> bool {
        self.current_step >= level_data.dangos_chain[self.current_wave].len()
    }

    pub fn start_next_wave(&mut self) -> bool {
        if self.current_wave + 1 < self.total_waves {
            self.current_wave += 1;
            self.current_step = 0;
            self.timer = 0.0;
            true
        } else {
            false
        }
    }

    pub fn all_waves_done(&self, level_data: &LevelData) -> bool {
        self.current_wave + 1 >= self.total_waves && self.is_wave_done(level_data)
    }
}

/// System: spawn enemies based on wave timing.
pub fn wave_spawn_system(
    time: Res<Time>,
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    level_data: Option<Res<LevelData>>,
    dango_configs: Res<DangoConfigs>,
    mut wave_mgr: ResMut<WaveManager>,
) {
    let Some(level_data) = level_data else {
        return;
    };

    if !wave_mgr.active {
        return;
    }

    let wave = wave_mgr.current_wave;
    if wave >= level_data.dangos_chain.len() {
        return;
    }

    let step = wave_mgr.current_step;
    if step >= level_data.dangos_chain[wave].len() {
        return;
    }

    wave_mgr.timer += time.delta_secs_f64();
    let spawn_delay = level_data.dangos_time[wave][step];

    if wave_mgr.timer >= spawn_delay {
        wave_mgr.timer = 0.0;

        // Parse dango type: "dangosimple0" → type="dangosimple", level=0
        let dango_str = &level_data.dangos_chain[wave][step];
        let (dango_type, dango_level) = parse_dango_string(dango_str);

        // Get the path for this enemy
        let path_idx = level_data.dangos_path[wave][step];
        let path_data = &level_data.paths[path_idx.min(level_data.paths.len() - 1)];
        let waypoints: Vec<Vec2> = path_data
            .path
            .iter()
            .map(|p| Vec2::new(p[0] as f32, p[1] as f32))
            .collect();

        let start_pos = waypoints
            .first()
            .copied()
            .unwrap_or(Vec2::ZERO);

        // Look up dango config
        let (hp, speed, attack, reload, xp, sugar) = if let Some(cfg) =
            dango_configs.0.get(&dango_type)
        {
            let lvl = dango_level.min(cfg.hitpoints.len().saturating_sub(1));
            (
                cfg.hitpoints.get(lvl).copied().unwrap_or(10.0),
                cfg.speed.get(lvl).copied().unwrap_or(1.0),
                cfg.attack.get(lvl).copied().unwrap_or(1.0),
                cfg.reload.get(lvl).copied().unwrap_or(1.0),
                cfg.xp.get(lvl).copied().unwrap_or(5.0),
                cfg.holy_sugar
                    .as_ref()
                    .and_then(|hs| hs.number.first().copied())
                    .unwrap_or(0),
            )
        } else {
            warn!("Unknown dango type: {}", dango_type);
            (10.0, 1.0, 1.0, 1.0, 5.0, 0)
        };

        // Spawn the enemy entity
        commands.spawn((
            Sprite::from_image(
                asset_server.load(format!(
                    "res/dangos/{}/idle.png",
                    dango_type
                )),
            ),
            Transform::from_translation(start_pos.extend(5.0)),
            Dango,
            DangoState::Moving,
            DangoStats {
                max_hp: hp,
                hp,
                prospective_damage: 0.0,
                speed,
                attack_damage: attack,
                attack_reload: reload,
                xp_reward: xp,
                sugar_reward: sugar,
                dango_type: dango_type.clone(),
                level: dango_level,
            },
            PathFollower::new(waypoints),
        ));

        wave_mgr.current_step += 1;
        wave_mgr.total_spawned += 1;
    }
}

/// Parse "dangosimple0" into ("dangosimple", 0).
fn parse_dango_string(s: &str) -> (String, usize) {
    // Last character is the level digit
    if let Some(last) = s.chars().last() {
        if let Some(level) = last.to_digit(10) {
            let name = &s[..s.len() - 1];
            return (name.to_string(), level as usize);
        }
    }
    (s.to_string(), 0)
}

/// System: auto-advance waves when current wave is fully spawned and all enemies are dead.
pub fn wave_advance_system(
    level_data: Option<Res<LevelData>>,
    mut wave_mgr: ResMut<WaveManager>,
    enemy_query: Query<Entity, With<Dango>>,
) {
    let Some(level_data) = level_data else {
        return;
    };

    if !wave_mgr.active {
        return;
    }

    // Check if current wave is fully spawned and no enemies remain
    if wave_mgr.is_wave_done(&level_data) && enemy_query.is_empty() {
        wave_mgr.start_next_wave();
    }
}
