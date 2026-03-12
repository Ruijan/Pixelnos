use bevy::prelude::*;
use mallows_vs_dango::config::ConfigPlugin;
use mallows_vs_dango::states::StatesPlugin;

/// Design resolution matching the original Cocos2d game.
const DESIGN_WIDTH: f32 = 1280.0;
const DESIGN_HEIGHT: f32 = 720.0;

fn main() {
    App::new()
        .add_plugins(
            DefaultPlugins
                .set(WindowPlugin {
                    primary_window: Some(Window {
                        title: "Mallows vs Dango".to_string(),
                        resolution: (DESIGN_WIDTH, DESIGN_HEIGHT).into(),
                        ..default()
                    }),
                    ..default()
                })
                .set(ImagePlugin::default_nearest()),
        )
        .add_plugins(ConfigPlugin)
        .add_plugins(StatesPlugin)
        .add_systems(Startup, setup)
        .run();
}

fn setup(mut commands: Commands, asset_server: Res<AssetServer>) {
    // Camera
    commands.spawn(Camera2d);

    // Display a background sprite to verify rendering works
    commands.spawn((
        Sprite::from_image(asset_server.load("res/background/menu_background1.png")),
        Transform::from_translation(Vec3::ZERO),
    ));

    // Display a text label overlay
    commands.spawn((
        Text::new("Mallows vs Dango - Phase 0"),
        TextFont {
            font: asset_server.load("fonts/Marker Felt.ttf"),
            font_size: 36.0,
            ..default()
        },
        TextColor(Color::WHITE),
        Node {
            position_type: PositionType::Absolute,
            top: Val::Px(20.0),
            left: Val::Px(20.0),
            ..default()
        },
    ));

    info!("Phase 0 setup complete: camera, background sprite, and text label spawned");
}
