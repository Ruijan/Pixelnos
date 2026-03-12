use bevy::prelude::*;
use mallows_vs_dango::audio::AudioPlugin;
use mallows_vs_dango::config::ConfigPlugin;
use mallows_vs_dango::scenes::{CreditsPlugin, MainMenuPlugin, StoryMenuPlugin};
use mallows_vs_dango::states::StatesPlugin;
use mallows_vs_dango::ui::TransitionPlugin;

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
        // Core systems
        .add_plugins(ConfigPlugin)
        .add_plugins(StatesPlugin)
        .add_plugins(AudioPlugin)
        .add_plugins(TransitionPlugin)
        // Scene plugins
        .add_plugins(MainMenuPlugin)
        .add_plugins(CreditsPlugin)
        .add_plugins(StoryMenuPlugin)
        // Global setup
        .add_systems(Startup, setup_camera)
        .run();
}

fn setup_camera(mut commands: Commands) {
    commands.spawn(Camera2d);
}
