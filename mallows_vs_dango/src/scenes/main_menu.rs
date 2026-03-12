use bevy::prelude::*;

use crate::audio::AudioManager;
use crate::config::GameConfig;
use crate::states::AppState;
use crate::ui::helpers::{cleanup_scene, SceneRoot, SpawnButtonExt};
use crate::ui::transition::SceneTransition;

pub struct MainMenuPlugin;

impl Plugin for MainMenuPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(OnEnter(AppState::MainMenu), setup_main_menu)
            .add_systems(OnExit(AppState::MainMenu), cleanup_scene)
            .add_systems(
                Update,
                handle_main_menu_buttons.run_if(in_state(AppState::MainMenu)),
            );
    }
}

#[derive(Component)]
struct PlayButton;
#[derive(Component)]
struct CreditsButton;
#[derive(Component)]
struct SkillsButton;

fn setup_main_menu(
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    game_config: Res<GameConfig>,
    mut audio: ResMut<AudioManager>,
) {
    info!("Setting up MainMenu scene");

    // Start menu music
    let menu_music = &game_config.sound_transition[0][0];
    if menu_music != "none" {
        audio.play_music(&mut commands, &asset_server, menu_music);
    }

    commands
        .spawn((
            Node {
                width: Val::Percent(100.0),
                height: Val::Percent(100.0),
                flex_direction: FlexDirection::Column,
                align_items: AlignItems::Center,
                justify_content: JustifyContent::Center,
                ..default()
            },
            SceneRoot,
        ))
        .with_children(|parent| {
            // Background
            parent.spawn((
                ImageNode::new(
                    asset_server.load("res/background/menu_background_battle.png"),
                ),
                Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    position_type: PositionType::Absolute,
                    ..default()
                },
            ));

            // Logo area (top 45%)
            parent
                .spawn(Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(45.0),
                    flex_direction: FlexDirection::Column,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::Center,
                    ..default()
                })
                .with_children(|logo_area| {
                    logo_area.spawn((
                        ImageNode::new(
                            asset_server.load("res/background/mallowstextgreen.png"),
                        ),
                        Node {
                            width: Val::Px(400.0),
                            height: Val::Px(80.0),
                            margin: UiRect::bottom(Val::Px(4.0)),
                            ..default()
                        },
                    ));
                    logo_area.spawn((
                        ImageNode::new(asset_server.load("res/background/vs_bubble.png")),
                        Node {
                            width: Val::Px(80.0),
                            height: Val::Px(80.0),
                            margin: UiRect::vertical(Val::Px(4.0)),
                            ..default()
                        },
                    ));
                    logo_area.spawn((
                        ImageNode::new(asset_server.load("res/background/dangotext2.png")),
                        Node {
                            width: Val::Px(300.0),
                            height: Val::Px(80.0),
                            margin: UiRect::top(Val::Px(4.0)),
                            ..default()
                        },
                    ));
                });

            // Button area (bottom 55%)
            parent
                .spawn(Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(55.0),
                    flex_direction: FlexDirection::Column,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::Start,
                    padding: UiRect::top(Val::Px(20.0)),
                    ..default()
                })
                .with_children(|button_area| {
                    button_area
                        .spawn_text_button(
                            &asset_server,
                            "START",
                            42.0,
                            Val::Px(280.0),
                            Val::Px(70.0),
                        )
                        .insert(PlayButton);

                    button_area
                        .spawn_text_button(
                            &asset_server,
                            "Talents",
                            28.0,
                            Val::Px(220.0),
                            Val::Px(55.0),
                        )
                        .insert(SkillsButton);

                    button_area
                        .spawn_text_button(
                            &asset_server,
                            "Credits",
                            28.0,
                            Val::Px(220.0),
                            Val::Px(55.0),
                        )
                        .insert(CreditsButton);
                });
        });
}

fn handle_main_menu_buttons(
    mut transition: ResMut<SceneTransition>,
    play_query: Query<&Interaction, (Changed<Interaction>, With<PlayButton>)>,
    credits_query: Query<&Interaction, (Changed<Interaction>, With<CreditsButton>)>,
    skills_query: Query<&Interaction, (Changed<Interaction>, With<SkillsButton>)>,
) {
    for interaction in &play_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::StoryMenu);
        }
    }
    for interaction in &credits_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::Credits);
        }
    }
    for interaction in &skills_query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::SkillTree);
        }
    }
}
