use bevy::prelude::*;

/// A projectile traveling toward a target.
#[derive(Component)]
pub struct Projectile {
    pub target: Entity,
    pub speed: f32,
    pub damage: f32,
    /// Cached last-known target position (used if target despawns mid-flight).
    pub target_pos: Vec2,
}

/// Marker for cleanup.
#[derive(Component)]
pub struct ProjectileCleanup;

/// System: move projectiles toward their targets.
pub fn projectile_movement_system(
    time: Res<Time>,
    mut commands: Commands,
    mut proj_query: Query<(Entity, &mut Transform, &mut Projectile), With<ProjectileCleanup>>,
    target_query: Query<&Transform, Without<ProjectileCleanup>>,
) {
    let dt = time.delta_secs();

    for (entity, mut proj_tf, mut proj) in &mut proj_query {
        // Update target position if target still exists
        if let Ok(target_tf) = target_query.get(proj.target) {
            proj.target_pos = target_tf.translation.truncate();
        }

        let current = proj_tf.translation.truncate();
        let to_target = proj.target_pos - current;
        let dist = to_target.length();
        let step = proj.speed * dt;

        if dist <= step {
            // Reached target — despawn projectile
            commands.entity(entity).despawn();
        } else {
            let dir = to_target / dist;
            proj_tf.translation.x += dir.x * step;
            proj_tf.translation.y += dir.y * step;

            // Rotate sprite to face target
            let angle = dir.y.atan2(dir.x);
            proj_tf.rotation = Quat::from_rotation_z(angle);
        }
    }
}
