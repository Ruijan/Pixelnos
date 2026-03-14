use std::collections::HashMap;
use std::fs;

// We test the raw serde deserialization without Bevy's ECS.
// The types are re-imported from the main crate.
use mallows_vs_dango::config::*;

fn read_config<T: serde::de::DeserializeOwned>(path: &str) -> T {
    let content = fs::read_to_string(format!("assets/{}", path))
        .unwrap_or_else(|e| panic!("Failed to read {}: {}", path, e));
    serde_json::from_str(&content).unwrap_or_else(|e| panic!("Failed to parse {}: {}", path, e))
}

#[test]
fn test_game_config_deserialize() {
    let config: GameConfig = read_config("res/config.json");
    assert_eq!(config.languages, vec!["en", "fr"]);
    assert!(config.play_dialogue);
    assert!(config.play_sound);
    assert_eq!(config.sound_transition.len(), 8);
    assert_eq!(config.wall.max_hp, 5);
    assert_eq!(config.wall.sprites.len(), 2);
    assert!(!config.configuration_files.tower.is_empty());
}

#[test]
fn test_tower_configs_deserialize() {
    let towers: HashMap<String, TowerConfig> = read_config("res/config_towers.json");
    assert_eq!(towers.len(), 3);
    assert!(towers.contains_key("bomber"));
    assert!(towers.contains_key("cutter"));
    assert!(towers.contains_key("saucer"));

    let bomber = &towers["bomber"];
    assert_eq!(bomber.name, "bomber");
    assert_eq!(bomber.cost.len(), 4);
    assert_eq!(bomber.damages.len(), 4);
    assert_eq!(bomber.range.len(), 4);

    let saucer = &towers["saucer"];
    assert!(saucer.slow.is_some());
    assert!(saucer.slow_duration.is_some());

    let cutter = &towers["cutter"];
    assert!(cutter.deep_wound_duration.is_some());
    assert!(cutter.deep_wound_percent.is_some());
}

#[test]
fn test_dango_configs_deserialize() {
    let dangos: HashMap<String, DangoConfig> = read_config("res/config_dangos.json");
    assert_eq!(dangos.len(), 3);
    assert!(dangos.contains_key("dangosimple"));
    assert!(dangos.contains_key("dangobese"));
    assert!(dangos.contains_key("dangorille"));

    let simple = &dangos["dangosimple"];
    assert_eq!(simple.name, "Dango");
    assert_eq!(simple.hitpoints.len(), 4);
    assert_eq!(simple.image.len(), 4);
    assert!(!simple.stay_on_ground);

    let obese = &dangos["dangobese"];
    assert!(obese.stay_on_ground);
    assert!(obese.attack_spe.is_some());

    let gorille = &dangos["dangorille"];
    assert!(gorille.attack_spe.is_some());
}

#[test]
fn test_level_configs_deserialize() {
    let levels: LevelConfigs = read_config("res/config_levels.json");
    assert!(!levels.worlds.is_empty());

    let world1 = &levels.worlds[0];
    assert_eq!(world1.id, 1);
    assert!(world1.name_en.is_some());
    assert!(!world1.levels.is_empty());

    let level1 = &world1.levels[0];
    assert_eq!(level1.id, 1);
    assert_eq!(level1.sugar, 90);
    assert!(level1.background.is_some());
    assert!(!level1.challenges.is_empty());
}

#[test]
fn test_challenge_configs_deserialize() {
    let challenges: HashMap<String, ChallengeConfig> = read_config("res/config_challenges.json");
    assert!(challenges.len() >= 8);
    assert!(challenges.contains_key("Untouchable"));
    assert!(challenges.contains_key("Misanthrope"));

    let untouchable = &challenges["Untouchable"];
    assert!(!untouchable.name_en.is_empty());
    assert!(!untouchable.image.is_empty());
}

#[test]
fn test_talent_configs_deserialize() {
    let talents: TalentConfigs = read_config("res/config_talents.json");
    assert!(!talents.talents.is_empty());

    let first = &talents.talents[0];
    assert_eq!(first.id, 1);
    assert!(!first.name_en.is_empty());
    assert!(first.cost > 0);
}

#[test]
fn test_advice_configs_deserialize() {
    let advice: HashMap<String, Vec<String>> = read_config("res/config_advice.json");
    assert!(advice.contains_key("en"));
    assert!(advice.contains_key("fr"));
    assert!(!advice["en"].is_empty());
}

#[test]
fn test_button_configs_deserialize() {
    let buttons: HashMap<String, LocalizedText> = read_config("res/config_buttons.json");
    assert!(buttons.contains_key("start"));
    assert!(buttons.contains_key("quit"));
    assert_eq!(buttons["start"].en, "Start");
}

#[test]
fn test_tutorial_configs_deserialize() {
    let tutorials: HashMap<String, TutorialConfig> =
        read_config("res/config_game_tutorials.json");
    assert!(tutorials.contains_key("tower_positioning"));
    assert!(tutorials.contains_key("sugar"));

    let sugar = &tutorials["sugar"];
    assert_eq!(sugar.state, "uncompleted");
    assert!(!sugar.dialogue.is_empty());
}

#[test]
fn test_skills_tutorial_configs_deserialize() {
    let tutorials: HashMap<String, SkillsTutorialConfig> =
        read_config("res/config_skills_tutorials.json");
    assert!(tutorials.contains_key("skills"));

    let skills = &tutorials["skills"];
    assert_eq!(skills.state, "uncompleted");
    assert!(!skills.dialogue.is_empty());
    assert!(!skills.dialogue_skills.is_empty());
}
