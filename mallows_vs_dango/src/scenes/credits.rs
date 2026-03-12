use bevy::prelude::*;

use crate::states::AppState;
use crate::ui::helpers::{cleanup_scene, SceneRoot, SpawnButtonExt};
use crate::ui::transition::SceneTransition;

pub struct CreditsPlugin;

impl Plugin for CreditsPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(OnEnter(AppState::Credits), setup_credits)
            .add_systems(OnExit(AppState::Credits), cleanup_scene)
            .add_systems(
                Update,
                (scroll_credits, handle_credits_buttons).run_if(in_state(AppState::Credits)),
            );
    }
}

#[derive(Component)]
struct CreditsScroll {
    speed: f32,
}

#[derive(Component)]
struct BackButton;

const CREDITS: &[(&str, &str)] = &[
    ("Game Developers", "Julien Rechenmann"),
    ("Game Designers", "Julien Rechenmann"),
    ("Designers", "Julien Rechenmann"),
    (
        "Music & Sounds",
        "Bensound.com\nSoundeffect-lab.info\npurple-planet.com",
    ),
    ("Graphic Helpers", "Subtlepatterns.com"),
    (
        "Acknowledgement",
        "Charline Loisil, Xianle Wang,\nKevin Passageon, Thomas Czereba,\nOmid Ahoura, Ann Pongsakul,\nGenia Shevchenko, Avril Li",
    ),
    (
        "Softwares & Libraries",
        "Bevy Engine, TexturePacker,\nInkscape, GIMP, Spine",
    ),
];

fn setup_credits(mut commands: Commands, asset_server: Res<AssetServer>) {
    info!("Setting up Credits scene");

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
                ImageNode::new(asset_server.load("res/background/crissXcross.png")),
                Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    position_type: PositionType::Absolute,
                    ..default()
                },
            ));

            // Back button (top-right)
            parent
                .spawn_image_button(&asset_server, "res/buttons/back.png", 48.0)
                .insert((
                    BackButton,
                    Node {
                        width: Val::Px(48.0),
                        height: Val::Px(48.0),
                        position_type: PositionType::Absolute,
                        top: Val::Px(16.0),
                        right: Val::Px(16.0),
                        ..default()
                    },
                ));

            // Scrolling credits container
            parent
                .spawn((
                    Node {
                        width: Val::Percent(100.0),
                        flex_direction: FlexDirection::Column,
                        align_items: AlignItems::Center,
                        padding: UiRect::top(Val::Vh(100.0)),
                        ..default()
                    },
                    CreditsScroll { speed: 50.0 },
                ))
                .with_children(|scroll| {
                    // Title
                    scroll.spawn((
                        Text::new("Credits"),
                        TextFont {
                            font: asset_server
                                .load("fonts/Love Is Complicated Again.ttf"),
                            font_size: 56.0,
                            ..default()
                        },
                        TextColor(Color::srgb(1.0, 0.6, 0.0)),
                        Node {
                            margin: UiRect::bottom(Val::Px(40.0)),
                            ..default()
                        },
                    ));

                    for (subtitle, content) in CREDITS {
                        scroll.spawn((
                            Text::new(*subtitle),
                            TextFont {
                                font: asset_server
                                    .load("fonts/Love Is Complicated Again.ttf"),
                                font_size: 38.0,
                                ..default()
                            },
                            TextColor(Color::srgb(1.0, 0.6, 0.0)),
                            Node {
                                margin: UiRect::new(
                                    Val::ZERO,
                                    Val::ZERO,
                                    Val::Px(30.0),
                                    Val::Px(8.0),
                                ),
                                ..default()
                            },
                        ));
                        scroll.spawn((
                            Text::new(*content),
                            TextFont {
                                font: asset_server
                                    .load("fonts/Love Is Complicated Again.ttf"),
                                font_size: 28.0,
                                ..default()
                            },
                            TextColor(Color::WHITE),
                            Node {
                                margin: UiRect::bottom(Val::Px(16.0)),
                                ..default()
                            },
                            TextLayout {
                                justify: JustifyText::Center,
                                ..default()
                            },
                        ));
                    }

                    // Bottom spacer
                    scroll.spawn(Node {
                        height: Val::Vh(60.0),
                        ..default()
                    });
                });
        });
}

fn scroll_credits(
    time: Res<Time>,
    mut query: Query<(&mut Node, &CreditsScroll)>,
    mut transition: ResMut<SceneTransition>,
) {
    for (mut node, scroll) in &mut query {
        if let Val::Vh(ref mut top) = node.padding.top {
            *top -= scroll.speed * time.delta_secs();
            if *top < -300.0 {
                transition.go_to(AppState::MainMenu);
            }
        }
    }
}

fn handle_credits_buttons(
    mut transition: ResMut<SceneTransition>,
    query: Query<&Interaction, (Changed<Interaction>, With<BackButton>)>,
) {
    for interaction in &query {
        if *interaction == Interaction::Pressed {
            transition.go_to(AppState::MainMenu);
        }
    }
}
