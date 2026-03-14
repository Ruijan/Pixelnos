use bevy::prelude::*;

/// Top-level application states corresponding to Cocos2d scenes.
#[derive(States, Default, Clone, Eq, PartialEq, Debug, Hash)]
pub enum AppState {
    #[default]
    Loading,
    MainMenu,
    StoryMenu,
    SkillTree,
    Shop,
    Credits,
    InGame,
    LevelEditor,
}

/// Sub-states active only when `AppState::InGame`.
#[derive(SubStates, Clone, Eq, PartialEq, Debug, Hash)]
#[source(AppState = AppState::InGame)]
pub enum GamePhase {
    LoadingLevel,
    PreWave,
    Playing,
    Paused,
    Won,
    Lost,
}

impl Default for GamePhase {
    fn default() -> Self {
        Self::LoadingLevel
    }
}

pub struct StatesPlugin;

impl Plugin for StatesPlugin {
    fn build(&self, app: &mut App) {
        app.init_state::<AppState>()
            .add_sub_state::<GamePhase>()
            // Transition from Loading -> MainMenu once configs are loaded
            .add_systems(OnEnter(AppState::Loading), transition_to_main_menu);
    }
}

fn transition_to_main_menu(mut next_state: ResMut<NextState<AppState>>) {
    info!("Configs loaded, transitioning to MainMenu");
    next_state.set(AppState::MainMenu);
}
