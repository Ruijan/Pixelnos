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
            .add_systems(OnEnter(AppState::Loading), on_enter_loading)
            .add_systems(OnEnter(AppState::MainMenu), on_enter_main_menu)
            .add_systems(OnEnter(AppState::StoryMenu), on_enter_story_menu)
            .add_systems(OnEnter(AppState::SkillTree), on_enter_skill_tree)
            .add_systems(OnEnter(AppState::Shop), on_enter_shop)
            .add_systems(OnEnter(AppState::Credits), on_enter_credits)
            .add_systems(OnEnter(AppState::InGame), on_enter_in_game)
            .add_systems(OnEnter(AppState::LevelEditor), on_enter_level_editor);
    }
}

fn on_enter_loading() {
    info!("Entering Loading state");
}

fn on_enter_main_menu() {
    info!("Entering MainMenu state");
}

fn on_enter_story_menu() {
    info!("Entering StoryMenu state");
}

fn on_enter_skill_tree() {
    info!("Entering SkillTree state");
}

fn on_enter_shop() {
    info!("Entering Shop state");
}

fn on_enter_credits() {
    info!("Entering Credits state");
}

fn on_enter_in_game() {
    info!("Entering InGame state");
}

fn on_enter_level_editor() {
    info!("Entering LevelEditor state");
}
