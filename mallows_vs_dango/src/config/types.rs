use bevy::prelude::*;
use serde::Deserialize;
use std::collections::HashMap;

// ---------------------------------------------------------------------------
// Root config (config.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct GameConfig {
    pub languages: Vec<String>,
    pub play_dialogue: bool,
    pub play_sound: bool,
    /// 2D matrix of music paths indexed by [from_scene][to_scene].
    /// "none" means no music change.
    pub sound_transition: Vec<Vec<String>>,
    pub wall: WallConfig,
    pub configuration_files: ConfigurationFiles,
}

#[derive(Debug, Deserialize)]
pub struct WallConfig {
    pub max_hp: u32,
    pub sprites: Vec<String>,
}

#[derive(Debug, Deserialize)]
pub struct ConfigurationFiles {
    pub tower: String,
    pub dango: String,
    pub challenge: String,
    pub talent: String,
    pub button: String,
    pub advice: String,
    pub level: String,
    #[serde(rename = "gameTutorial")]
    pub game_tutorial: String,
    #[serde(rename = "skillsTutorial")]
    pub skills_tutorial: String,
}

// ---------------------------------------------------------------------------
// Tower configs (config_towers.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct TowerConfigs(pub HashMap<String, TowerConfig>);

#[derive(Debug, Deserialize)]
pub struct TowerConfig {
    pub skeleton: String,
    pub atlas: String,
    pub animation_bullet: String,
    #[serde(default)]
    pub animation_bullet_size: f32,
    pub attack_speed: Vec<f32>,
    pub cost: Vec<f32>,
    pub sell: Vec<f32>,
    pub range: Vec<f32>,
    pub damages: Vec<f32>,
    pub xp_level: Vec<f32>,
    pub animation_size: AnimationSize,
    pub image: String,
    pub description_en: String,
    pub description_fr: String,
    #[serde(default)]
    pub last_level_description_en: Option<String>,
    #[serde(default)]
    pub last_level_description_fr: Option<String>,
    pub name: String,
    #[serde(default)]
    pub unlock_level: Option<i32>,
    #[serde(default)]
    pub unlock_world: Option<i32>,
    #[serde(default)]
    pub unlock_skill: Option<i32>,
    #[serde(default)]
    pub limit: u32,
    #[serde(default)]
    pub limit_skill_id: u32,
    pub sound_bullet: String,
    // Saucer-specific
    #[serde(default)]
    pub slow: Option<Vec<f32>>,
    #[serde(default)]
    pub slow_duration: Option<Vec<f32>>,
    // Cutter-specific
    #[serde(default)]
    pub deep_wound_duration: Option<f32>,
    #[serde(default)]
    pub deep_wound_percent: Option<f32>,
}

#[derive(Debug, Deserialize)]
pub struct AnimationSize {
    pub width: f32,
    pub height: f32,
}

// ---------------------------------------------------------------------------
// Dango configs (config_dangos.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct DangoConfigs(pub HashMap<String, DangoConfig>);

#[derive(Debug, Deserialize)]
pub struct DangoConfig {
    pub cellperanim: f32,
    pub name: String,
    pub skeleton: String,
    pub atlas: String,
    #[serde(default)]
    pub stay_on_ground: bool,
    #[serde(default)]
    pub description_en: String,
    #[serde(default)]
    pub description_fr: String,
    pub attack: Vec<f32>,
    pub gain: Vec<f32>,
    pub hitpoints: Vec<f32>,
    pub image: Vec<String>,
    pub names: Vec<String>,
    pub reload: Vec<f32>,
    pub speed: Vec<f32>,
    pub xp: Vec<f32>,
    #[serde(default)]
    pub holy_sugar: Option<HolySugarConfig>,
    #[serde(default)]
    pub attack_spe: Option<serde_json::Value>,
}

#[derive(Debug, Deserialize)]
pub struct HolySugarConfig {
    pub prob: Vec<f32>,
    pub number: Vec<u32>,
    #[serde(rename = "type")]
    pub kind: Vec<String>,
}

// ---------------------------------------------------------------------------
// Level configs (config_levels.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct LevelConfigs {
    pub worlds: Vec<WorldConfig>,
}

#[derive(Debug, Deserialize)]
pub struct WorldConfig {
    pub id: u32,
    #[serde(default)]
    pub name_en: Option<String>,
    #[serde(default)]
    pub name_fr: Option<String>,
    #[serde(default)]
    pub levels: Vec<LevelConfig>,
}

#[derive(Debug, Deserialize)]
pub struct LevelConfig {
    pub id: u32,
    #[serde(default)]
    pub dialogues_file: Option<String>,
    #[serde(default)]
    pub animations: Vec<String>,
    #[serde(default)]
    pub background: Option<String>,
    #[serde(default)]
    pub effects: Vec<String>,
    #[serde(default)]
    pub exp: u32,
    #[serde(default)]
    pub holy_sugar: u32,
    #[serde(default)]
    pub finalized: bool,
    #[serde(default)]
    pub path_level: Option<String>,
    #[serde(default)]
    pub musics: Vec<String>,
    #[serde(default)]
    pub reward: Option<RewardConfig>,
    #[serde(default)]
    pub sugar: u32,
    #[serde(default, rename = "type")]
    pub level_type: Option<String>,
    #[serde(default)]
    pub tileset: Vec<String>,
    #[serde(default)]
    pub tilesetpng: Vec<String>,
    #[serde(default)]
    pub x: f32,
    #[serde(default)]
    pub y: f32,
    #[serde(default)]
    pub challenges: Vec<ChallengeRef>,
}

#[derive(Debug, Deserialize)]
pub struct RewardConfig {
    #[serde(rename = "type")]
    pub reward_type: String,
    #[serde(default)]
    pub file: Option<String>,
    #[serde(default)]
    pub tower_name: Option<String>,
}

#[derive(Debug, Deserialize)]
pub struct ChallengeRef {
    pub name: String,
    #[serde(default)]
    pub int_value: i32,
    #[serde(default)]
    pub str_value: String,
}

// ---------------------------------------------------------------------------
// Challenge configs (config_challenges.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct ChallengeConfigs(pub HashMap<String, ChallengeConfig>);

#[derive(Debug, Deserialize)]
pub struct ChallengeConfig {
    pub name_fr: String,
    pub name_en: String,
    pub description_fr: String,
    pub description_en: String,
    pub image: String,
}

// ---------------------------------------------------------------------------
// Talent configs (config_talents.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct TalentConfigs {
    pub talents: Vec<TalentConfig>,
}

#[derive(Debug, Deserialize)]
pub struct TalentConfig {
    pub id: u32,
    pub name_en: String,
    pub name_fr: String,
    pub sprite_enabled: String,
    pub sprite_disabled: String,
    pub description_en: String,
    pub description_fr: String,
    pub condition_id: i32,
    pub condition_sugar: i32,
    pub condition_level: i32,
    #[serde(default)]
    pub condition_world: i32,
    pub cost: u32,
    #[serde(default)]
    pub bonus: Option<f32>,
}

// ---------------------------------------------------------------------------
// Advice configs (config_advice.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct AdviceConfigs(pub HashMap<String, Vec<String>>);

// ---------------------------------------------------------------------------
// Button configs (config_buttons.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct ButtonConfigs(pub HashMap<String, LocalizedText>);

#[derive(Debug, Deserialize)]
pub struct LocalizedText {
    pub en: String,
    pub fr: String,
}

// ---------------------------------------------------------------------------
// Tutorial configs (config_game_tutorials.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct GameTutorialConfigs(pub HashMap<String, TutorialConfig>);

#[derive(Debug, Deserialize)]
pub struct TutorialConfig {
    pub state: String,
    #[serde(default)]
    pub dialogue: Vec<DialogueEntry>,
    #[serde(default)]
    pub level: Option<u32>,
    #[serde(default)]
    pub world: Option<u32>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct DialogueEntry {
    pub emotion: Vec<String>,
    pub side: String,
    pub speaker: String,
    pub text_en: Vec<String>,
    pub text_fr: Vec<String>,
}

// ---------------------------------------------------------------------------
// Skills tutorial configs (config_skills_tutorials.json)
// ---------------------------------------------------------------------------

#[derive(Debug, Deserialize, Resource)]
pub struct SkillsTutorialConfigs(pub HashMap<String, SkillsTutorialConfig>);

#[derive(Debug, Deserialize)]
pub struct SkillsTutorialConfig {
    pub state: String,
    #[serde(default)]
    pub dialogue: Vec<DialogueEntry>,
    #[serde(default)]
    pub dialogue_skills: Vec<DialogueEntry>,
    #[serde(default)]
    pub level: Option<u32>,
    #[serde(default)]
    pub world: Option<u32>,
}
