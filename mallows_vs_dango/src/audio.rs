use bevy::prelude::*;

/// Manages background music and sound effects, mirroring the original AudioController.
pub struct AudioPlugin;

impl Plugin for AudioPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<AudioManager>()
            .add_systems(Update, music_fade_system);
    }
}

/// Central audio resource replacing the C++ AudioController singleton.
#[derive(Resource)]
pub struct AudioManager {
    pub music_volume: f32,
    pub effects_volume: f32,
    pub music_enabled: bool,
    pub effects_enabled: bool,
    pub loop_enabled: bool,
    /// Currently playing music track path.
    pub current_music: Option<String>,
    /// Entity holding the current music AudioPlayer.
    pub music_entity: Option<Entity>,
    /// Active fade operation.
    pub fade: Option<MusicFade>,
}

impl Default for AudioManager {
    fn default() -> Self {
        Self {
            music_volume: 0.5,
            effects_volume: 0.5,
            music_enabled: true,
            effects_enabled: true,
            loop_enabled: true,
            current_music: None,
            music_entity: None,
            fade: None,
        }
    }
}

/// Describes a music crossfade operation.
pub struct MusicFade {
    pub phase: FadePhase,
    pub timer: Timer,
    pub next_track: Option<String>,
}

#[derive(PartialEq)]
pub enum FadePhase {
    FadingOut,
    FadingIn,
}

impl AudioManager {
    /// Start playing music, crossfading from the current track.
    pub fn play_music(
        &mut self,
        commands: &mut Commands,
        asset_server: &AssetServer,
        track: &str,
    ) {
        // If same track is already playing, do nothing
        if self.current_music.as_deref() == Some(track) {
            return;
        }

        if !self.music_enabled {
            self.current_music = Some(track.to_string());
            return;
        }

        if self.music_entity.is_some() {
            // Crossfade: start fading out, then switch
            self.fade = Some(MusicFade {
                phase: FadePhase::FadingOut,
                timer: Timer::from_seconds(0.5, TimerMode::Once),
                next_track: Some(track.to_string()),
            });
        } else {
            // No music playing, just start
            self.spawn_music(commands, asset_server, track);
        }
    }

    /// Spawn a new music entity.
    fn spawn_music(
        &mut self,
        commands: &mut Commands,
        asset_server: &AssetServer,
        track: &str,
    ) {
        let source = asset_server.load(track);
        let entity = commands
            .spawn((
                AudioPlayer::new(source),
                PlaybackSettings {
                    mode: if self.loop_enabled {
                        bevy::audio::PlaybackMode::Loop
                    } else {
                        bevy::audio::PlaybackMode::Once
                    },
                    volume: bevy::audio::Volume::new(self.music_volume),
                    ..default()
                },
                MusicTrack,
            ))
            .id();

        self.music_entity = Some(entity);
        self.current_music = Some(track.to_string());

        // Start fade-in
        self.fade = Some(MusicFade {
            phase: FadePhase::FadingIn,
            timer: Timer::from_seconds(0.5, TimerMode::Once),
            next_track: None,
        });
    }

    /// Play a sound effect.
    pub fn play_effect(
        &mut self,
        commands: &mut Commands,
        asset_server: &AssetServer,
        effect: &str,
    ) {
        if !self.effects_enabled {
            return;
        }
        let source = asset_server.load(effect);
        commands.spawn((
            AudioPlayer::new(source),
            PlaybackSettings {
                mode: bevy::audio::PlaybackMode::Despawn,
                volume: bevy::audio::Volume::new(self.effects_volume),
                ..default()
            },
        ));
    }

    /// Stop all music.
    pub fn stop_music(&mut self, commands: &mut Commands) {
        if let Some(entity) = self.music_entity.take() {
            commands.entity(entity).despawn();
        }
        self.fade = None;
    }
}

/// Marker component for the music track entity.
#[derive(Component)]
pub struct MusicTrack;

/// System that handles music fade-in/fade-out each frame.
fn music_fade_system(
    mut audio: ResMut<AudioManager>,
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    time: Res<Time>,
    audio_sinks: Query<&AudioSink, With<MusicTrack>>,
) {
    // Extract fade state to avoid borrow conflicts
    let Some(mut fade) = audio.fade.take() else {
        return;
    };

    fade.timer.tick(time.delta());
    let progress = fade.timer.fraction();
    let music_volume = audio.music_volume;
    let music_entity = audio.music_entity;

    match fade.phase {
        FadePhase::FadingOut => {
            let volume = music_volume * (1.0 - progress);
            if let Some(entity) = music_entity {
                if let Ok(sink) = audio_sinks.get(entity) {
                    sink.set_volume(volume);
                }
            }

            if fade.timer.finished() {
                if let Some(entity) = audio.music_entity.take() {
                    commands.entity(entity).despawn();
                }
                let next_track = fade.next_track.clone();
                if let Some(track) = next_track {
                    audio.spawn_music(&mut commands, &asset_server, &track);
                }
                // spawn_music sets a new fade, don't overwrite it
                return;
            }
        }
        FadePhase::FadingIn => {
            let volume = music_volume * progress;
            if let Some(entity) = music_entity {
                if let Ok(sink) = audio_sinks.get(entity) {
                    sink.set_volume(volume);
                }
            }

            if fade.timer.finished() {
                if let Some(entity) = music_entity {
                    if let Ok(sink) = audio_sinks.get(entity) {
                        sink.set_volume(music_volume);
                    }
                }
                // Fade complete, don't put it back
                return;
            }
        }
    }

    // Put fade back if not finished
    audio.fade = Some(fade);
}
