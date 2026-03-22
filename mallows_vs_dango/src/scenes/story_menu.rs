use bevy::prelude::*;

use crate::audio::AudioManager;
use crate::config::{GameConfig, LevelConfigs};
use crate::states::AppState;
use crate::ui::helpers::{cleanup_scene, SceneRoot, SpawnButtonExt};
use crate::ui::transition::SceneTransition;

pub struct StoryMenuPlugin;

impl Plugin for StoryMenuPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<StoryMenuState>()
            .add_systems(OnEnter(AppState::StoryMenu), setup_story_menu)
            .add_systems(OnExit(AppState::StoryMenu), cleanup_scene)
            .add_systems(
                Update,
                (
                    handle_story_menu_buttons,
                    handle_level_buttons,
                    handle_world_navigation,
                )
                    .run_if(in_state(AppState::StoryMenu)),
            );
    }
}

#[derive(Resource, Default)]
pub struct StoryMenuState {
    pub current_world: usize,
    pub selected_world_id: u32,
    pub selected_level_id: u32,
}

#[derive(Component)]
struct BackToMenuButton;
#[derive(Component)]
struct SkillTreeButton;
#[derive(Component)]
struct ShopButton;
#[derive(Component)]
struct PrevWorldButton;
#[derive(Component)]
struct NextWorldButton;
#[derive(Component)]
struct LevelButton {
    world_id: u32,
    level_id: u32,
}
#[derive(Component)]
struct WorldPage {
    index: usize,
}

fn setup_story_menu(
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    level_configs: Res<LevelConfigs>,
    game_config: Res<GameConfig>,
    mut audio: ResMut<AudioManager>,
    mut menu_state: ResMut<StoryMenuState>,
) {
    info!("Setting up StoryMenu scene");

    // Music for story menu (index 1 in transition matrix)
    let story_music = &game_config.sound_transition[1][1];
    if story_music != "none" {
        audio.play_music(&mut commands, &asset_server, story_music);
    }

    menu_state.current_world = 0;

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
            // Background
            parent.spawn((
                ImageNode::new(asset_server.load("res/background/space.png")),
                Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    position_type: PositionType::Absolute,
                    ..default()
                },
            ));

            // Top bar
            parent
                .spawn(Node {
                    width: Val::Percent(100.0),
                    height: Val::Px(60.0),
                    position_type: PositionType::Absolute,
                    top: Val::Px(8.0),
                    flex_direction: FlexDirection::Row,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::SpaceBetween,
                    padding: UiRect::horizontal(Val::Px(16.0)),
                    ..default()
                })
                .with_children(|top_bar| {
                    // Holy sugar display (left)
                    top_bar
                        .spawn(Node {
                            flex_direction: FlexDirection::Row,
                            align_items: AlignItems::Center,
                            ..default()
                        })
                        .with_children(|sugar| {
                            sugar.spawn((
                                ImageNode::new(
                                    asset_server.load("res/buttons/holy_sugar.png"),
                                ),
                                Node {
                                    width: Val::Px(40.0),
                                    height: Val::Px(40.0),
                                    margin: UiRect::right(Val::Px(8.0)),
                                    ..default()
                                },
                            ));
                            sugar.spawn((
                                Text::new("x 0"),
                                TextFont {
                                    font: asset_server.load("fonts/LICABOLD.ttf"),
                                    font_size: 24.0,
                                    ..default()
                                },
                                TextColor(Color::srgb(0.95, 0.85, 0.2)),
                            ));
                        });

                    // Right side buttons
                    top_bar
                        .spawn(Node {
                            flex_direction: FlexDirection::Row,
                            align_items: AlignItems::Center,
                            ..default()
                        })
                        .with_children(|buttons| {
                            buttons
                                .spawn_image_button(
                                    &asset_server,
                                    "res/buttons/menu_button_skill_tree.png",
                                    50.0,
                                )
                                .insert(SkillTreeButton);

                            buttons
                                .spawn_image_button(
                                    &asset_server,
                                    "res/buttons/menu_button_shop.png",
                                    50.0,
                                )
                                .insert(ShopButton);

                            buttons
                                .spawn_image_button(
                                    &asset_server,
                                    "res/buttons/back.png",
                                    50.0,
                                )
                                .insert(BackToMenuButton);
                        });
                });

            // World page container (center)
            parent
                .spawn(Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    position_type: PositionType::Absolute,
                    flex_direction: FlexDirection::Column,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::Center,
                    ..default()
                })
                .with_children(|center| {
                    for (world_idx, world) in level_configs.worlds.iter().enumerate() {
                        if world.levels.is_empty() {
                            continue;
                        }

                        let world_name =
                            world.name_en.as_deref().unwrap_or("Unknown World");

                        center
                            .spawn((
                                Node {
                                    width: Val::Percent(65.0),
                                    height: Val::Percent(75.0),
                                    flex_direction: FlexDirection::Column,
                                    align_items: AlignItems::Center,
                                    justify_content: JustifyContent::Start,
                                    padding: UiRect::all(Val::Px(16.0)),
                                    display: if world_idx == 0 {
                                        Display::Flex
                                    } else {
                                        Display::None
                                    },
                                    ..default()
                                },
                                WorldPage { index: world_idx },
                            ))
                            .with_children(|page| {
                                // World background
                                page.spawn((
                                    ImageNode::new(
                                        asset_server.load("res/background/levels2.png"),
                                    ),
                                    Node {
                                        width: Val::Percent(100.0),
                                        height: Val::Percent(100.0),
                                        position_type: PositionType::Absolute,
                                        ..default()
                                    },
                                ));

                                // World name
                                page.spawn((
                                    Text::new(world_name),
                                    TextFont {
                                        font: asset_server.load("fonts/LICABOLD.ttf"),
                                        font_size: 32.0,
                                        ..default()
                                    },
                                    TextColor(Color::srgb(0.95, 0.85, 0.2)),
                                    Node {
                                        margin: UiRect::bottom(Val::Px(16.0)),
                                        ..default()
                                    },
                                ));

                                // Level buttons grid
                                page.spawn(Node {
                                    width: Val::Percent(100.0),
                                    height: Val::Percent(85.0),
                                    flex_direction: FlexDirection::Row,
                                    flex_wrap: FlexWrap::Wrap,
                                    justify_content: JustifyContent::Center,
                                    align_items: AlignItems::Center,
                                    align_content: AlignContent::Center,
                                    ..default()
                                })
                                .with_children(|grid| {
                                    for level in &world.levels {
                                        let level_type = level
                                            .level_type
                                            .as_deref()
                                            .unwrap_or("normal");
                                        let button_image = if level_type == "boss" {
                                            "res/buttons/level_button_pink.png"
                                        } else {
                                            "res/buttons/level_button.png"
                                        };

                                        let img =
                                            asset_server.load(button_image);
                                        let font =
                                            asset_server.load("fonts/LICABOLD.ttf");
                                        let level_id = level.id;
                                        let w_id = world.id;

                                        grid.spawn((
                                            Button,
                                            Node {
                                                width: Val::Px(55.0),
                                                height: Val::Px(55.0),
                                                margin: UiRect::all(Val::Px(6.0)),
                                                justify_content:
                                                    JustifyContent::Center,
                                                align_items: AlignItems::Center,
                                                ..default()
                                            },
                                            BackgroundColor(Color::srgba(
                                                0.0, 0.0, 0.0, 0.0,
                                            )),
                                            LevelButton {
                                                world_id: w_id,
                                                level_id,
                                            },
                                        ))
                                        .with_children(|btn| {
                                            btn.spawn((
                                                ImageNode::new(img),
                                                Node {
                                                    width: Val::Percent(100.0),
                                                    height: Val::Percent(100.0),
                                                    position_type:
                                                        PositionType::Absolute,
                                                    ..default()
                                                },
                                            ));
                                            btn.spawn((
                                                Text::new(format!("{}", level_id)),
                                                TextFont {
                                                    font,
                                                    font_size: 20.0,
                                                    ..default()
                                                },
                                                TextColor(Color::WHITE),
                                            ));
                                        });
                                    }
                                });
                            });
                    }
                });

            // World navigation arrows
            parent
                .spawn_image_button(
                    &asset_server,
                    "res/buttons/next_world_button.png",
                    60.0,
                )
                .insert((
                    PrevWorldButton,
                    Node {
                        width: Val::Px(60.0),
                        height: Val::Px(60.0),
                        position_type: PositionType::Absolute,
                        left: Val::Px(16.0),
                        top: Val::Percent(45.0),
                        ..default()
                    },
                ));

            parent
                .spawn_image_button(
                    &asset_server,
                    "res/buttons/next_world_button.png",
                    60.0,
                )
                .insert((
                    NextWorldButton,
                    Node {
                        width: Val::Px(60.0),
                        height: Val::Px(60.0),
                        position_type: PositionType::Absolute,
                        right: Val::Px(16.0),
                        top: Val::Percent(45.0),
                        ..default()
                    },
                ));
        });
}

fn handle_story_menu_buttons(
    mut transition: ResMut<SceneTransition>,
    back_query: Query<&Interaction, (Changed<Interaction>, With<BackToMenuButton>)>,
    skill_query: Query<&Interaction, (Changed<Interaction>, With<SkillTreeButton>)>,
    shop_query: Query<&Interaction, (Changed<Interaction>, With<ShopButton>)>,
) {
    for interaction in &back_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::MainMenu);
        }
    }
    for interaction in &skill_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::SkillTree);
        }
    }
    for interaction in &shop_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::Shop);
        }
    }
}

fn handle_level_buttons(
    mut commands: Commands,
    mut menu_state: ResMut<StoryMenuState>,
    mut transition: ResMut<SceneTransition>,
    query: Query<(&Interaction, &LevelButton), Changed<Interaction>>,
) {
    for (interaction, level_btn) in &query {
        if *interaction == Interaction::Pressed {
            info!(
                "Level selected: world {}, level {}",
                level_btn.world_id, level_btn.level_id
            );
            menu_state.selected_world_id = level_btn.world_id;
            menu_state.selected_level_id = level_btn.level_id;

            // Set SelectedLevel resource and transition to InGame
            commands.insert_resource(
                crate::game::game_scene::SelectedLevel {
                    world_id: level_btn.world_id,
                    level_id: level_btn.level_id,
                },
            );
            transition.go_to(AppState::InGame);
        }
    }
}

fn handle_world_navigation(
    mut menu_state: ResMut<StoryMenuState>,
    level_configs: Res<LevelConfigs>,
    prev_query: Query<&Interaction, (Changed<Interaction>, With<PrevWorldButton>)>,
    next_query: Query<&Interaction, (Changed<Interaction>, With<NextWorldButton>)>,
    mut page_query: Query<(&mut Node, &WorldPage)>,
) {
    let world_count = level_configs
        .worlds
        .iter()
        .filter(|w| !w.levels.is_empty())
        .count();

    let mut changed = false;

    for interaction in &prev_query {
        if *interaction == Interaction::Pressed && menu_state.current_world > 0 {
            menu_state.current_world -= 1;
            changed = true;
        }
    }
    for interaction in &next_query {
        if *interaction == Interaction::Pressed && menu_state.current_world + 1 < world_count {
            menu_state.current_world += 1;
            changed = true;
        }
    }

    if changed {
        for (mut node, page) in &mut page_query {
            node.display = if page.index == menu_state.current_world {
                Display::Flex
            } else {
                Display::None
            };
        }
    }
}
