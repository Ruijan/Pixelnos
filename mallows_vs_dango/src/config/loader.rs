use bevy::prelude::*;
use std::fs;
use std::path::Path;

use super::types::*;

pub struct ConfigPlugin;

impl Plugin for ConfigPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(PreStartup, load_all_configs);
    }
}

fn load_and_parse<T: serde::de::DeserializeOwned>(path: &str) -> T {
    let full_path = Path::new("assets").join(path);
    let content = fs::read_to_string(&full_path)
        .unwrap_or_else(|e| panic!("Failed to read config {}: {}", full_path.display(), e));
    serde_json::from_str(&content)
        .unwrap_or_else(|e| panic!("Failed to parse config {}: {}", full_path.display(), e))
}

fn load_all_configs(mut commands: Commands) {
    info!("Loading game configurations...");

    // Load root config first (it tells us where the other files are)
    let game_config: GameConfig = load_and_parse("res/config.json");
    info!(
        "  Root config loaded: {} languages, {} sound transitions",
        game_config.languages.len(),
        game_config.sound_transition.len()
    );

    // Load all sub-configs using paths from root config
    let tower_configs: TowerConfigs = {
        let map = load_and_parse(&game_config.configuration_files.tower);
        TowerConfigs(map)
    };
    info!("  Tower configs loaded: {} types", tower_configs.0.len());

    let dango_configs: DangoConfigs = {
        let map = load_and_parse(&game_config.configuration_files.dango);
        DangoConfigs(map)
    };
    info!("  Dango configs loaded: {} types", dango_configs.0.len());

    let level_configs: LevelConfigs = load_and_parse(&game_config.configuration_files.level);
    let total_levels: usize = level_configs.worlds.iter().map(|w| w.levels.len()).sum();
    info!(
        "  Level configs loaded: {} worlds, {} levels",
        level_configs.worlds.len(),
        total_levels
    );

    let challenge_configs: ChallengeConfigs = {
        let map = load_and_parse(&game_config.configuration_files.challenge);
        ChallengeConfigs(map)
    };
    info!(
        "  Challenge configs loaded: {} types",
        challenge_configs.0.len()
    );

    let talent_configs: TalentConfigs = load_and_parse(&game_config.configuration_files.talent);
    info!(
        "  Talent configs loaded: {} talents",
        talent_configs.talents.len()
    );

    let advice_configs: AdviceConfigs = {
        let map = load_and_parse(&game_config.configuration_files.advice);
        AdviceConfigs(map)
    };
    info!(
        "  Advice configs loaded: {} languages",
        advice_configs.0.len()
    );

    let button_configs: ButtonConfigs = {
        let map = load_and_parse(&game_config.configuration_files.button);
        ButtonConfigs(map)
    };
    info!(
        "  Button configs loaded: {} entries",
        button_configs.0.len()
    );

    let tutorial_configs: GameTutorialConfigs = {
        let map = load_and_parse(&game_config.configuration_files.game_tutorial);
        GameTutorialConfigs(map)
    };
    info!(
        "  Game tutorial configs loaded: {} tutorials",
        tutorial_configs.0.len()
    );

    let skills_tutorial_configs: SkillsTutorialConfigs = {
        let map = load_and_parse(&game_config.configuration_files.skills_tutorial);
        SkillsTutorialConfigs(map)
    };
    info!(
        "  Skills tutorial configs loaded: {} entries",
        skills_tutorial_configs.0.len()
    );

    // Insert all configs as Bevy resources
    commands.insert_resource(game_config);
    commands.insert_resource(tower_configs);
    commands.insert_resource(dango_configs);
    commands.insert_resource(level_configs);
    commands.insert_resource(challenge_configs);
    commands.insert_resource(talent_configs);
    commands.insert_resource(advice_configs);
    commands.insert_resource(button_configs);
    commands.insert_resource(tutorial_configs);
    commands.insert_resource(skills_tutorial_configs);

    info!("All configs loaded successfully!");
}
