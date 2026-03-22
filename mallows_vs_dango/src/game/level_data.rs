use bevy::prelude::*;
use serde::Deserialize;

/// Per-level JSON data loaded from `res/levels/config/level_N.json`.
#[derive(Debug, Deserialize, Resource, Clone)]
pub struct LevelData {
    pub level_name: String,
    pub nbwaves: usize,

    /// Wave definitions: `dangosChain[wave][step]` = dango type string (e.g. "dangosimple0").
    #[serde(rename = "dangosChain")]
    pub dangos_chain: Vec<Vec<String>>,

    /// Spawn timing: `dangosTime[wave][step]` = delay in seconds between spawns.
    #[serde(rename = "dangosTime")]
    pub dangos_time: Vec<Vec<f64>>,

    /// Path indices: `dangosPath[wave][step]` = which path this enemy uses.
    #[serde(rename = "dangosPath")]
    pub dangos_path: Vec<Vec<usize>>,

    /// Named paths. Each path is a list of [x, y] waypoints in world-space.
    pub paths: Vec<PathData>,

    /// Background sprites for the level.
    #[serde(default)]
    pub backgrounds: Vec<LevelObject>,

    /// Decorative object sprites.
    #[serde(default)]
    pub objects: Vec<LevelObject>,

    /// Design resolution of the level editor when this level was created.
    #[serde(default)]
    pub resolution: Option<LevelResolution>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct PathData {
    pub path_name: String,
    /// List of `[x, y]` waypoints.
    pub path: Vec<[f64; 2]>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct LevelObject {
    pub image_name: String,
    pub position: [f64; 2],
    #[serde(default)]
    pub rotation: f64,
    pub scale: [f64; 2],
    #[serde(default)]
    pub zorder: i32,
    #[serde(default)]
    pub flipped: [bool; 2],
}

#[derive(Debug, Deserialize, Clone)]
pub struct LevelResolution {
    pub width: f64,
    pub height: f64,
}

impl LevelData {
    /// Load a level file from a JSON string.
    pub fn from_json(json: &str) -> Result<Self, serde_json::Error> {
        serde_json::from_str(json)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_level_data_deserialize() {
        let json = include_str!(
            "../../assets/res/levels/config/level_1.json"
        );
        let data = LevelData::from_json(json).expect("Failed to parse level_1.json");
        assert_eq!(data.nbwaves, 3);
        assert_eq!(data.paths.len(), 1);
        assert_eq!(data.dangos_chain.len(), 3);
        assert!(!data.paths[0].path.is_empty());
    }
}
