use bevy::prelude::*;

use crate::states::AppState;

pub struct TransitionPlugin;

impl Plugin for TransitionPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<SceneTransition>()
            .add_systems(Startup, spawn_fade_overlay)
            .add_systems(Update, transition_system);
    }
}

/// Resource that drives scene transitions with a fade-to-black overlay.
/// Mirrors the original SceneManager's TransitionFade (0.5s).
#[derive(Resource, Default)]
pub struct SceneTransition {
    pub state: TransitionState,
}

#[derive(Default, PartialEq)]
pub enum TransitionState {
    #[default]
    Idle,
    FadingOut {
        target: AppState,
        timer: Timer,
    },
    FadingIn {
        timer: Timer,
    },
}

impl SceneTransition {
    /// Start a transition to a new scene with fade-to-black.
    pub fn go_to(&mut self, target: AppState) {
        if self.state != TransitionState::Idle {
            return; // Already transitioning
        }
        self.state = TransitionState::FadingOut {
            target,
            timer: Timer::from_seconds(0.25, TimerMode::Once),
        };
    }
}

/// Marker for the persistent fade overlay entity.
#[derive(Component)]
pub struct FadeOverlay;

fn spawn_fade_overlay(mut commands: Commands) {
    // Full-screen black overlay, starts transparent, drawn on top of everything
    commands.spawn((
        Node {
            width: Val::Vw(100.0),
            height: Val::Vh(100.0),
            position_type: PositionType::Absolute,
            ..default()
        },
        BackgroundColor(Color::srgba(0.0, 0.0, 0.0, 0.0)),
        // High z-index so it's on top of all UI
        ZIndex(999),
        FadeOverlay,
    ));
}

fn transition_system(
    mut transition: ResMut<SceneTransition>,
    mut next_state: ResMut<NextState<AppState>>,
    time: Res<Time>,
    mut overlay_query: Query<&mut BackgroundColor, With<FadeOverlay>>,
) {
    let mut bg = overlay_query.single_mut();

    match &mut transition.state {
        TransitionState::Idle => {
            // Ensure overlay is fully transparent
            bg.0 = Color::srgba(0.0, 0.0, 0.0, 0.0);
        }
        TransitionState::FadingOut { target, timer } => {
            timer.tick(time.delta());
            let alpha = timer.fraction();
            bg.0 = Color::srgba(0.0, 0.0, 0.0, alpha);

            if timer.finished() {
                // Switch state while screen is black
                next_state.set(target.clone());
                transition.state = TransitionState::FadingIn {
                    timer: Timer::from_seconds(0.25, TimerMode::Once),
                };
            }
        }
        TransitionState::FadingIn { timer } => {
            timer.tick(time.delta());
            let alpha = 1.0 - timer.fraction();
            bg.0 = Color::srgba(0.0, 0.0, 0.0, alpha);

            if timer.finished() {
                transition.state = TransitionState::Idle;
            }
        }
    }
}
