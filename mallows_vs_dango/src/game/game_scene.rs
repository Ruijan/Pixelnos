use bevy::prelude::*;

use crate::audio::AudioManager;
use crate::config::{GameConfig, LevelConfigs, TowerConfigs};
use crate::states::AppState;
use crate::ui::helpers::{SceneRoot, SpawnButtonExt};
use crate::ui::transition::SceneTransition;

use super::enemy;
use super::grid::Grid;
use super::level_data::LevelData;
use super::projectile;
use super::tower::{
    self, AttackTimer, GridPosition, Tower, TowerState, TowerStats, TowerTarget,
};
use super::wave::WaveManager;

pub struct GameScenePlugin;

impl Plugin for GameScenePlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<WaveManager>()
            .init_resource::<GameLives>()
            .init_resource::<GameSugar>()
            .add_systems(OnEnter(AppState::InGame), setup_game)
            .add_systems(OnExit(AppState::InGame), cleanup_game)
            .add_systems(
                Update,
                (
                    // Enemy systems
                    enemy::enemy_movement_system,
                    enemy::enemy_damage_system,
                    enemy::enemy_cleanup_system,
                    // Tower systems
                    tower::tower_targeting_system,
                    tower::tower_attack_system,
                    // Projectile systems
                    projectile::projectile_movement_system,
                    // Wave systems
                    super::wave::wave_spawn_system,
                    super::wave::wave_advance_system,
                    // Game UI
                    update_hud,
                    handle_game_buttons,
                    check_win_lose,
                )
                    .run_if(in_state(AppState::InGame)),
            );
    }
}

/// Player lives (health).
#[derive(Resource)]
pub struct GameLives {
    pub current: u32,
    pub max: u32,
}

impl Default for GameLives {
    fn default() -> Self {
        Self {
            current: 20,
            max: 20,
        }
    }
}

/// Sugar currency available to build towers.
#[derive(Resource)]
pub struct GameSugar {
    pub amount: u32,
}

impl Default for GameSugar {
    fn default() -> Self {
        Self { amount: 100 }
    }
}

// === Selected level tracking ===

/// Resource set before entering InGame to indicate which level to load.
#[derive(Resource, Default)]
pub struct SelectedLevel {
    pub world_id: u32,
    pub level_id: u32,
}

// === UI marker components ===

#[derive(Component)]
struct HudLivesText;
#[derive(Component)]
struct HudSugarText;
#[derive(Component)]
struct HudWaveText;
#[derive(Component)]
struct StartWaveButton;
#[derive(Component)]
struct BackButton;
#[derive(Component)]
struct TowerBuyButton {
    tower_type: String,
}

fn setup_game(
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    level_configs: Res<LevelConfigs>,
    tower_configs: Res<TowerConfigs>,
    _game_config: Res<GameConfig>,
    mut audio: ResMut<AudioManager>,
    windows: Query<&Window>,
    selected: Option<Res<SelectedLevel>>,
    mut lives: ResMut<GameLives>,
    mut sugar: ResMut<GameSugar>,
    mut wave_mgr: ResMut<WaveManager>,
) {
    info!("Setting up InGame scene");

    // Reset game state
    *lives = GameLives::default();
    *sugar = GameSugar::default();

    let sel = selected
        .as_ref()
        .map(|s| (s.world_id, s.level_id))
        .unwrap_or((1, 1));

    // Find the level config
    let level_cfg = level_configs
        .worlds
        .iter()
        .find(|w| w.id == sel.0)
        .and_then(|w| w.levels.iter().find(|l| l.id == sel.1));

    // Get initial sugar from level config
    if let Some(cfg) = level_cfg {
        sugar.amount = cfg.sugar;
    }

    // Load level data JSON
    let level_path = level_cfg
        .and_then(|c| c.path_level.as_deref())
        .unwrap_or("res/levels/config/level_1.json");

    let level_json_path = format!(
        "{}/mallows_vs_dango/assets/{}",
        env!("CARGO_MANIFEST_DIR").rsplit_once("/mallows_vs_dango").map(|(p, _)| p).unwrap_or("."),
        level_path
    );

    // Try to load level data synchronously from file
    let level_data = match std::fs::read_to_string(&level_json_path) {
        Ok(json) => match LevelData::from_json(&json) {
            Ok(data) => Some(data),
            Err(e) => {
                warn!("Failed to parse level JSON: {}", e);
                None
            }
        },
        Err(_) => {
            // Try relative path
            match std::fs::read_to_string(format!("assets/{}", level_path)) {
                Ok(json) => LevelData::from_json(&json).ok(),
                Err(e) => {
                    warn!("Failed to load level file {}: {}", level_path, e);
                    None
                }
            }
        }
    };

    let level_data = level_data.unwrap_or_else(|| {
        warn!("Using fallback level data");
        LevelData {
            level_name: "Fallback Level".to_string(),
            nbwaves: 1,
            dangos_chain: vec![vec!["dangosimple0".to_string(); 5]],
            dangos_time: vec![vec![2.0; 5]],
            dangos_path: vec![vec![0; 5]],
            paths: vec![super::level_data::PathData {
                path_name: "fallback".to_string(),
                path: vec![[-500.0, 0.0], [500.0, 0.0]],
            }],
            backgrounds: vec![],
            objects: vec![],
            resolution: None,
        }
    });

    // Initialize wave manager
    *wave_mgr = WaveManager::new(level_data.nbwaves);

    // Build grid
    let window = windows.single();
    let mut grid = Grid::new(window.width(), window.height());

    // Mark path cells
    for path_data in &level_data.paths {
        grid.mark_path(&path_data.path);
    }

    commands.insert_resource(grid);
    commands.insert_resource(level_data.clone());

    // Music
    if let Some(cfg) = level_cfg {
        if let Some(music) = cfg.musics.first() {
            audio.play_music(&mut commands, &asset_server, music);
        }
    }

    // === Spawn background sprites ===
    for bg in &level_data.backgrounds {
        commands.spawn((
            Sprite::from_image(asset_server.load(&bg.image_name)),
            Transform {
                translation: Vec3::new(bg.position[0] as f32, bg.position[1] as f32, bg.zorder as f32 * 0.1),
                rotation: Quat::from_rotation_z(bg.rotation.to_radians() as f32),
                scale: Vec3::new(
                    bg.scale[0] as f32 * if bg.flipped[0] { -1.0 } else { 1.0 },
                    bg.scale[1] as f32 * if bg.flipped[1] { -1.0 } else { 1.0 },
                    1.0,
                ),
            },
            SceneRoot,
        ));
    }

    // === Spawn object sprites ===
    for obj in &level_data.objects {
        // Objects reference atlas sprites — use the image name directly
        // The atlas system will resolve these in a future phase
        let image_path = if obj.image_name.contains('/') {
            obj.image_name.clone()
        } else {
            format!("res/levels/objects/{}", obj.image_name)
        };

        commands.spawn((
            Sprite::from_image(asset_server.load(&image_path)),
            Transform {
                translation: Vec3::new(
                    obj.position[0] as f32,
                    obj.position[1] as f32,
                    1.0 + obj.zorder as f32 * 0.1,
                ),
                rotation: Quat::from_rotation_z((obj.rotation as f32).to_radians()),
                scale: Vec3::new(
                    obj.scale[0] as f32 * if obj.flipped[0] { -1.0 } else { 1.0 },
                    obj.scale[1] as f32 * if obj.flipped[1] { -1.0 } else { 1.0 },
                    1.0,
                ),
            },
            SceneRoot,
        ));
    }

    // === HUD ===
    commands
        .spawn((
            Node {
                width: Val::Percent(100.0),
                height: Val::Percent(100.0),
                ..default()
            },
            SceneRoot,
        ))
        .with_children(|parent| {
            // Top bar with lives, sugar, wave info
            parent
                .spawn(Node {
                    width: Val::Percent(100.0),
                    height: Val::Px(50.0),
                    position_type: PositionType::Absolute,
                    top: Val::Px(0.0),
                    flex_direction: FlexDirection::Row,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::SpaceBetween,
                    padding: UiRect::horizontal(Val::Px(16.0)),
                    ..default()
                })
                .with_children(|top| {
                    // Lives
                    top.spawn(Node {
                        flex_direction: FlexDirection::Row,
                        align_items: AlignItems::Center,
                        ..default()
                    })
                    .with_children(|lives_area| {
                        lives_area.spawn((
                            Text::new(format!("Lives: {}", lives.current)),
                            TextFont {
                                font: asset_server.load("fonts/LICABOLD.ttf"),
                                font_size: 22.0,
                                ..default()
                            },
                            TextColor(Color::srgb(1.0, 0.3, 0.3)),
                            HudLivesText,
                        ));
                    });

                    // Sugar
                    top.spawn(Node {
                        flex_direction: FlexDirection::Row,
                        align_items: AlignItems::Center,
                        ..default()
                    })
                    .with_children(|sugar_area| {
                        sugar_area.spawn((
                            ImageNode::new(
                                asset_server.load("res/buttons/holy_sugar.png"),
                            ),
                            Node {
                                width: Val::Px(30.0),
                                height: Val::Px(30.0),
                                margin: UiRect::right(Val::Px(6.0)),
                                ..default()
                            },
                        ));
                        sugar_area.spawn((
                            Text::new(format!("{}", sugar.amount)),
                            TextFont {
                                font: asset_server.load("fonts/LICABOLD.ttf"),
                                font_size: 22.0,
                                ..default()
                            },
                            TextColor(Color::srgb(0.95, 0.85, 0.2)),
                            HudSugarText,
                        ));
                    });

                    // Wave counter
                    top.spawn((
                        Text::new(format!(
                            "Wave {}/{}",
                            wave_mgr.current_wave + 1,
                            wave_mgr.total_waves
                        )),
                        TextFont {
                            font: asset_server.load("fonts/LICABOLD.ttf"),
                            font_size: 22.0,
                            ..default()
                        },
                        TextColor(Color::WHITE),
                        HudWaveText,
                    ));

                    // Back button
                    top.spawn_image_button(&asset_server, "res/buttons/back.png", 40.0)
                        .insert(BackButton);
                });

            // Right panel: tower buy buttons
            parent
                .spawn(Node {
                    width: Val::Px(80.0),
                    height: Val::Percent(60.0),
                    position_type: PositionType::Absolute,
                    right: Val::Px(8.0),
                    top: Val::Percent(20.0),
                    flex_direction: FlexDirection::Column,
                    align_items: AlignItems::Center,
                    ..default()
                })
                .with_children(|panel| {
                    // Create a buy button for each tower type
                    let mut sorted_towers: Vec<_> = tower_configs.0.iter().collect();
                    sorted_towers.sort_by_key(|(name, _)| (*name).clone());

                    for (tower_name, cfg) in &sorted_towers {
                        panel
                            .spawn_image_button(
                                &asset_server,
                                &cfg.image,
                                60.0,
                            )
                            .insert(TowerBuyButton {
                                tower_type: tower_name.to_string(),
                            });
                    }
                });

            // Start wave button (bottom center)
            parent
                .spawn_text_button(
                    &asset_server,
                    "Start Wave",
                    24.0,
                    Val::Px(180.0),
                    Val::Px(50.0),
                )
                .insert((
                    StartWaveButton,
                    Node {
                        width: Val::Px(180.0),
                        height: Val::Px(50.0),
                        position_type: PositionType::Absolute,
                        bottom: Val::Px(16.0),
                        left: Val::Percent(50.0),
                        margin: UiRect::left(Val::Px(-90.0)),
                        justify_content: JustifyContent::Center,
                        align_items: AlignItems::Center,
                        ..default()
                    },
                ));
        });
}

fn cleanup_game(
    mut commands: Commands,
    query: Query<Entity, With<SceneRoot>>,
) {
    for entity in &query {
        commands.entity(entity).despawn_recursive();
    }
    commands.remove_resource::<LevelData>();
    commands.remove_resource::<Grid>();
}

fn update_hud(
    lives: Res<GameLives>,
    sugar: Res<GameSugar>,
    wave_mgr: Res<WaveManager>,
    mut lives_text: Query<&mut Text, (With<HudLivesText>, Without<HudSugarText>, Without<HudWaveText>)>,
    mut sugar_text: Query<&mut Text, (With<HudSugarText>, Without<HudLivesText>, Without<HudWaveText>)>,
    mut wave_text: Query<&mut Text, (With<HudWaveText>, Without<HudLivesText>, Without<HudSugarText>)>,
) {
    for mut text in &mut lives_text {
        **text = format!("Lives: {}", lives.current);
    }
    for mut text in &mut sugar_text {
        **text = format!("{}", sugar.amount);
    }
    for mut text in &mut wave_text {
        **text = format!("Wave {}/{}", wave_mgr.current_wave + 1, wave_mgr.total_waves);
    }
}

fn handle_game_buttons(
    mut transition: ResMut<SceneTransition>,
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    tower_configs: Res<TowerConfigs>,
    mut wave_mgr: ResMut<WaveManager>,
    mut sugar: ResMut<GameSugar>,
    grid: Option<Res<Grid>>,
    back_query: Query<&Interaction, (Changed<Interaction>, With<BackButton>)>,
    start_query: Query<&Interaction, (Changed<Interaction>, With<StartWaveButton>)>,
    tower_buy_query: Query<
        (&Interaction, &TowerBuyButton),
        Changed<Interaction>,
    >,
) {
    for interaction in &back_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::StoryMenu);
        }
    }

    for interaction in &start_query {
        if *interaction == Interaction::Pressed && !wave_mgr.active {
            wave_mgr.active = true;
            info!("Wave {} started!", wave_mgr.current_wave + 1);
        }
    }

    let Some(grid) = grid else { return };

    for (interaction, buy_btn) in &tower_buy_query {
        if *interaction != Interaction::Pressed {
            continue;
        }

        let Some(cfg) = tower_configs.0.get(&buy_btn.tower_type) else {
            continue;
        };

        let cost = cfg.cost.first().copied().unwrap_or(50.0) as u32;
        if sugar.amount < cost {
            info!("Not enough sugar to buy {}", buy_btn.tower_type);
            continue;
        }

        // Find a free cell near the center of the grid
        let center = grid.grid_to_world(
            super::grid::GRID_COLS / 2,
            super::grid::GRID_ROWS / 2,
        );
        let Some((col, row)) = grid.nearest_free_cell(center) else {
            info!("No free cell to place tower");
            continue;
        };

        sugar.amount -= cost;
        let pos = grid.grid_to_world(col, row);

        commands.spawn((
            Sprite::from_image(asset_server.load(&cfg.image)),
            Transform::from_translation(pos.extend(8.0))
                .with_scale(Vec3::splat(0.5)),
            Tower,
            TowerState::Idle,
            TowerStats {
                tower_type: buy_btn.tower_type.clone(),
                level: 0,
                damage: cfg.damages.first().copied().unwrap_or(10.0),
                range: cfg.range.first().copied().unwrap_or(150.0),
                attack_speed: cfg.attack_speed.first().copied().unwrap_or(1.0),
                cost: cfg.cost.first().copied().unwrap_or(50.0),
                xp: 0.0,
                xp_to_next: cfg.xp_level.first().copied().unwrap_or(100.0),
            },
            AttackTimer {
                timer: Timer::from_seconds(
                    cfg.attack_speed.first().copied().unwrap_or(1.0),
                    TimerMode::Repeating,
                ),
            },
            TowerTarget { target: None },
            GridPosition { col, row },
            SceneRoot,
        ));

        info!("Placed {} at ({}, {})", buy_btn.tower_type, col, row);
    }
}

fn check_win_lose(
    mut transition: ResMut<SceneTransition>,
    lives: Res<GameLives>,
    wave_mgr: Res<WaveManager>,
    level_data: Option<Res<LevelData>>,
    enemies: Query<Entity, With<enemy::Dango>>,
) {
    // Lose condition: no lives
    if lives.current == 0 {
        info!("Game Over — no lives remaining");
        transition.go_to(AppState::StoryMenu);
        return;
    }

    // Win condition: all waves done and no enemies alive
    let Some(level_data) = level_data else {
        return;
    };
    if wave_mgr.active
        && wave_mgr.all_waves_done(&level_data)
        && enemies.is_empty()
    {
        info!("Level Complete!");
        transition.go_to(AppState::StoryMenu);
    }
}
