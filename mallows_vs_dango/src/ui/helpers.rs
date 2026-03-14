use bevy::prelude::*;

/// Marker for entities that belong to a specific scene and should be
/// despawned on scene exit.
#[derive(Component)]
pub struct SceneRoot;

/// Despawn all entities with SceneRoot when leaving a state.
pub fn cleanup_scene(mut commands: Commands, query: Query<Entity, With<SceneRoot>>) {
    for entity in &query {
        commands.entity(entity).despawn_recursive();
    }
}

/// Extension trait to spawn styled buttons from a ChildBuilder.
pub trait SpawnButtonExt {
    fn spawn_text_button(
        &mut self,
        asset_server: &AssetServer,
        label: &str,
        font_size: f32,
        width: Val,
        height: Val,
    ) -> EntityCommands<'_>;

    fn spawn_image_button(
        &mut self,
        asset_server: &AssetServer,
        image_path: &str,
        size: f32,
    ) -> EntityCommands<'_>;
}

impl SpawnButtonExt for ChildBuilder<'_> {
    fn spawn_text_button(
        &mut self,
        asset_server: &AssetServer,
        label: &str,
        font_size: f32,
        width: Val,
        height: Val,
    ) -> EntityCommands<'_> {
        let font = asset_server.load("fonts/LICABOLD.ttf");
        let bg_image = asset_server.load("res/buttons/common_button_wood.png");
        let label = label.to_string();

        let mut ec = self.spawn((
            Button,
            Node {
                width,
                height,
                justify_content: JustifyContent::Center,
                align_items: AlignItems::Center,
                margin: UiRect::all(Val::Px(8.0)),
                ..default()
            },
            BackgroundColor(Color::srgba(0.0, 0.0, 0.0, 0.0)),
        ));

        ec.with_children(move |parent| {
            parent.spawn((
                ImageNode::new(bg_image),
                Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    position_type: PositionType::Absolute,
                    ..default()
                },
            ));
            parent.spawn((
                Text::new(label),
                TextFont {
                    font,
                    font_size,
                    ..default()
                },
                TextColor(Color::srgb(0.95, 0.85, 0.2)),
            ));
        });

        ec
    }

    fn spawn_image_button(
        &mut self,
        asset_server: &AssetServer,
        image_path: &str,
        size: f32,
    ) -> EntityCommands<'_> {
        let image = asset_server.load(image_path);

        let mut ec = self.spawn((
            Button,
            Node {
                width: Val::Px(size),
                height: Val::Px(size),
                margin: UiRect::all(Val::Px(4.0)),
                ..default()
            },
            BackgroundColor(Color::srgba(0.0, 0.0, 0.0, 0.0)),
        ));

        ec.with_children(move |parent| {
            parent.spawn((
                ImageNode::new(image),
                Node {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    ..default()
                },
            ));
        });

        ec
    }
}
