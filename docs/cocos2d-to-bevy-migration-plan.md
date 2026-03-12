# Cocos2d-x to Bevy Migration Plan

## MallowsVsDango: C++ to Rust Port

---

## Executive Summary

This document outlines a phased migration plan for porting **MallowsVsDango**, a 2D tower defense game, from C++ Cocos2d-x to Rust Bevy 0.15. The game consists of ~28,000 lines of C++ across 176 source files with 353 resource files (sprites, audio, fonts, JSON configs, Spine animations).

### Target
- **Engine:** Bevy 0.15 (latest stable)
- **Language:** Rust
- **Primary platform:** Desktop (Windows/Linux/macOS)
- **Secondary platforms:** Mobile (iOS/Android) and Web (WASM) considered for future
- **Animation:** `bevy_spine` crate for Spine runtime support

### Estimated Timeline
- **Solo developer:** 20-22 weeks
- **Team of 2-3:** 12-14 weeks

---

## 1. Feasibility Assessment

### What Maps Well to Bevy

| Cocos2d Concept | Bevy Equivalent | Difficulty |
|---|---|---|
| Scene graph (Node/Sprite hierarchy) | ECS entities with `Transform` + `Sprite` | Low |
| Scene transitions (Director) | Bevy `States` with `OnEnter`/`OnExit` systems | Low |
| Sprite rendering | `SpriteBundle`, `TextureAtlas` | Low |
| JSON config loading | `serde` + `serde_json` deserialization | Low |
| Touch/mouse input | Bevy `Input<MouseButton>`, `Touches` | Low |
| Audio playback | `bevy_audio` (or `bevy_kira_audio`) | Low |
| Scheduled updates (`update(float dt)`) | Bevy systems with `Time` resource | Low |
| Factory pattern (TowerFactory, etc.) | Builder functions or spawn helper systems | Low |
| Timer-based logic | Bevy `Timer` component | Low |
| Grid-based placement | Custom `Grid` resource + `Cell` components | Low |

### What Requires Significant Rework

| Cocos2d Concept | Challenge | Mitigation |
|---|---|---|
| Deep OOP inheritance (Tower -> Cutter/Saucer/Bomber) | Bevy ECS uses composition over inheritance | Decompose into components + marker types |
| Spine animations (`spine-cocos2dx`) | Third-party `bevy_spine` crate required | Validate early; spritesheet fallback ready |
| Cocos2d Actions system (Sequence, FadeIn, MoveTo) | No direct equivalent | Custom tween systems or `bevy_tweening` crate |
| cocos2d::ui widgets | `bevy_ui` is less feature-rich | Build small reusable widget library |
| Singleton pattern (Config, SceneManager, AudioController) | Anti-pattern in ECS | Use Bevy `Resource` types |
| HTTP networking (cocos2d HttpClient) | Must bridge async into ECS | `bevy::tasks::AsyncComputeTaskPool` + channels |
| DrawNode debug rendering | Different API | Bevy `Gizmos` system |
| Plist sprite sheets | Bevy uses `TextureAtlasLayout` | Write conversion script (one-time) |

### Feasibility Verdict: **HIGH**

The game is well-scoped for migration. The core gameplay loop (grid placement, path movement, targeting, combat) translates cleanly to ECS. The main risks are Spine animation support and UI complexity, both of which have viable fallbacks. The game has no physics engine dependency, no 3D rendering, and no complex shader requirements - all factors that simplify the port.

---

## 2. Architecture Transformation Guide

### From OOP Inheritance to ECS Composition

The original codebase uses deep class hierarchies. In Bevy ECS, these become component compositions.

#### Towers

**C++ (inheritance):**
```
Tower (base) -> Cutter
             -> Saucer
             -> Bomber
```

**Rust/Bevy (composition):**
```rust
// Shared components on ALL towers
#[derive(Component)] struct Tower;
#[derive(Component)] struct TowerState(State); // IDLE, AWARE, ATTACKING, ...
#[derive(Component)] struct TowerStats { range: f32, damage: f32, attack_speed: f32, cost: f32 }
#[derive(Component)] struct TowerLevel { current: u32, max: u32, xp: u32 }
#[derive(Component)] struct Target(Option<Entity>);

// Type-specific marker components
#[derive(Component)] struct Cutter;
#[derive(Component)] struct Saucer { slow_percent: f32 }
#[derive(Component)] struct Bomber { splash_radius: f32 }

// Systems query by marker:
fn cutter_attack_system(query: Query<(&Tower, &Target, &TowerStats), With<Cutter>>) { ... }
fn saucer_attack_system(query: Query<(&Tower, &Target, &SaucerData), With<Saucer>>) { ... }
fn bomber_attack_system(query: Query<(&Tower, &Target, &BomberData), With<Bomber>>) { ... }
```

#### Enemies (Dangos)

**C++ (inheritance):**
```
Dango (base) -> Dangosimple
             -> Dangobese
             -> Dangorille
             -> Monkey
```

**Rust/Bevy (composition):**
```rust
#[derive(Component)] struct Dango;
#[derive(Component)] struct DangoState(State); // IDLE, MOVE, ATTACK, DYING, DEAD
#[derive(Component)] struct DangoStats { hp: f32, max_hp: f32, speed: f32, damage: f32 }
#[derive(Component)] struct PathFollower { path_index: usize, waypoint_index: usize, progress: f32 }
#[derive(Component)] struct ActiveEffects(Vec<Effect>);

// Type-specific
#[derive(Component)] struct Dangobese;       // has ground-pound special
#[derive(Component)] struct Dangorille;      // summons monkeys
#[derive(Component)] struct Monkey { target_tower: Option<Entity> }
```

#### Attacks

**C++ (inheritance):**
```
Attack (base) -> Slash / DeepSlash (instant melee)
             -> ThrowableAttack -> ChocoSpit / WaterBall / WaterBombBall (projectiles)
```

**Rust/Bevy (composition):**
```rust
#[derive(Component)] struct Attack { damage: f32, source: Entity }
#[derive(Component)] struct MeleeAttack { aoe_range: f32 }
#[derive(Component)] struct Projectile { speed: f32, target: Entity }
#[derive(Component)] struct AreaOfEffect { radius: f32 }
#[derive(Component)] struct AppliesEffect(EffectType);
```

### Scenes to States

```rust
#[derive(States, Default, Clone, Eq, PartialEq, Debug, Hash)]
enum AppState {
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

#[derive(SubStates, Clone, Eq, PartialEq, Debug, Hash)]
#[source(AppState = AppState::InGame)]
enum GamePhase {
    LoadingLevel,
    PreWave,      // StartMenu overlay
    Playing,
    Paused,
    Won,
    Lost,
}
```

### Singletons to Resources

| C++ Singleton | Bevy Resource |
|---|---|
| `Config` | `Res<GameConfig>` (with sub-resources for each config type) |
| `SceneManager` | Bevy `States` + `NextState<AppState>` |
| `AudioController` | `Res<AudioManager>` |

---

## 3. Asset Pipeline

### Sprites (290 PNG files)
- **Action:** Copy directly to `assets/` directory. Bevy loads PNGs natively.
- **Organization:** Mirror the original `res/` folder structure under `assets/`.

### Sprite Sheets (8 plist/png pairs)
- **Action:** Write a one-time Python/Rust conversion script to convert `.plist` files to Bevy `TextureAtlasLayout` definitions.
- **Format:** Plist files contain frame rects, offsets, and source sizes. Convert to a JSON or RON file that a Bevy startup system loads.

### Spine Animations
- **Action:** Copy `.json` + `.atlas` + `.png` files for each skeleton. Load via `bevy_spine`.
- **Skeletons identified:** cutter, saucer, bomber (towers), dangosimple, dangobese, dangorille (enemies).
- **Fallback:** If `bevy_spine` is inadequate, export Spine animations to spritesheet sequences using Spine's official tool.

### Audio (8 files)
- **Action:** Copy to `assets/audio/`. Bevy supports OGG, WAV, MP3 natively.
- **Music crossfade:** Implement custom fade system (Bevy doesn't have built-in crossfade).

### Fonts (8 TTF/OTF files)
- **Action:** Copy to `assets/fonts/`. Bevy loads TTF natively for text rendering.

### JSON Configuration (30 files)
- **Action:** Define strongly-typed Rust structs with `serde::Deserialize`. Load at startup.
- **Key configs:** `config.json`, `config_towers.json`, `config_dangos.json`, `config_levels.json`, `config_challenges.json`, `config_talent.json`, `config_advice.json`, tutorial configs.
- **Benefit:** Compile-time type safety replaces runtime `Json::Value` access.

---

## 4. Migration Phases

---

### Phase 0: Project Scaffold and Asset Pipeline (Week 1)

**Goal:** Bevy project compiles, loads all configs, renders a test sprite.

**Tasks:**
1. Initialize a new Rust workspace: `cargo init --name mallows_vs_dango`
2. Add dependencies:
   ```toml
   [dependencies]
   bevy = "0.15"
   serde = { version = "1", features = ["derive"] }
   serde_json = "1"
   bevy_spine = "0.9"  # or latest compatible version
   ```
3. Copy all assets from `MallowsVsDango/Resources/` into `assets/`
4. Write plist-to-TextureAtlasLayout conversion script; run on all 8 plist files
5. Define all `AppState` variants with empty `OnEnter`/`OnExit` systems
6. Define typed Rust structs for all JSON configs; write a startup system that loads and validates them
7. Display a background sprite and text label to verify rendering
8. **Early Spine validation:** Load one Spine skeleton (e.g., `cutter.json` + atlas) using `bevy_spine` to confirm it works with the game's Spine format version

**Deliverable:** App launches, shows a sprite, prints config values to console.

**Tests:**
- Unit: JSON deserialization of every config file into typed structs
- Manual: Window opens, sprite renders, Spine skeleton loads without error

---

### Phase 1: Main Menu and Scene Navigation (Week 2-3)

**Goal:** Navigate between Menu, Credits, StoryMenu screens with fade transitions and music.

**Tasks:**
1. Implement `MainMenu` scene: background sprite, buttons (Play, Skills, Credits) using `bevy_ui`
2. Implement `CreditScreen`: scrolling text display
3. Implement `StoryMenu`: world/level buttons loaded from `config_levels.json`
4. Implement scene transition system: fade-to-black overlay entity, state change, fade-in
5. Port `AudioController` as an `AudioManager` resource:
   - `play_music()`, `play_effect()`, `set_volume()`, `fade_music()` methods
   - Wire up per-scene music from `sound_transition` config matrix

**Deliverable:** Click through Menu -> StoryMenu -> select level (no gameplay yet) -> back to Menu. Credits scroll. Music plays and crossfades between scenes.

**Tests:**
- Manual: All navigation paths work, no orphaned entities on scene change
- Use `bevy-inspector-egui` in dev builds to verify entity cleanup
- Automated: State transition test (enter state, verify expected entities exist)

---

### Phase 2: Grid, Cells, and Path System (Week 3-4)

**Goal:** Render a level's background, grid, paths, and decorative objects. No gameplay yet.

**Tasks:**
1. Define `Cell` component and `Grid` resource; implement `init_cells` system from level config
2. Parse `paths` array from level JSON; store as `PathData` resource (vec of vec of `Vec2`)
3. Render level backgrounds from config (`backgrounds` array)
4. Render decorative objects from config (`objects` array) with flipping, scaling, rotation, z-order
5. Implement debug grid visualization using Bevy `Gizmos`
6. Port `LoadingScreen` as a brief asset-loading state before entering the level

**Deliverable:** Select level 1 from StoryMenu, see the full level background with all objects and grid overlay.

**Tests:**
- Visual comparison with original game screenshots
- Unit: Grid dimensions and path waypoint counts match expected values from config

---

### Phase 3: Tower Placement and Basic Tower Logic (Week 4-6)

**Goal:** Place towers on the grid, basic state machine, tower management UI.

**Tasks:**
1. Define `Tower`, `TowerKind`, `TowerStats`, `TowerLevel` components
2. Implement `RightPanel` UI: three tower icons with drag-to-place interaction
3. Tower placement system: on drop, find nearest free cell, spawn tower entity, mark cell occupied
4. Tower rendering with static sprites initially (Spine comes later in Phase 6)
5. Tower state machine systems: IDLE (scan for enemies), AWARE (waiting), RELOADING (timer), ATTACKING (placeholder)
6. Tower selection: tap tower -> show info panel with stats, upgrade button, sell button
7. Port `LevelInfo` HUD: sugar count, life count, wave progress indicator
8. Sugar resource system: buying towers costs sugar, selling refunds partial cost

**Deliverable:** Place all 3 tower types on valid cells, see them on screen, select/upgrade/sell, sugar balance updates.

**Tests:**
- Unit: Tower placement on occupied cell rejected; sugar arithmetic correct; tower stat scaling per level matches config
- Integration: Place tower, verify entity has correct components
- Manual: Drag-and-drop feels responsive

---

### Phase 4: Enemy Spawning and Path Movement (Week 6-8)

**Goal:** Enemies spawn in waves and walk along paths. No combat yet.

**Tasks:**
1. Define `Dango`, `DangoKind`, `DangoStats`, `PathFollower` components
2. Port `DangoGenerator` as a `WaveController` resource: parse wave data (type, timing, path) from level JSON
3. Wave spawning system: timer-based, spawns dango entities at path start position
4. Path-following movement system: each dango moves toward next waypoint, interpolating position, handling direction changes
5. Dango rendering with static sprites (one per type per level)
6. Implement `StartMenu` overlay (the "Start" button that begins wave spawning)
7. Life system: dango reaching end of path reduces life; life = 0 triggers lose condition
8. Sugar gain: dangos that exit the map consume life but aren't killed; sugar awarded on actual kill

**Deliverable:** Press Start, watch waves of dangos walk the path. Life decreases when they reach the end. Wave progression works across all waves.

**Tests:**
- Unit: Wave timing accuracy; path following reaches each waypoint
- Integration: Spawn 10 enemies, all reach end, life decremented correctly
- Manual: Movement smooth at 60 FPS, correct speed per dango type

---

### Phase 5: Combat System (Week 8-11)

**Goal:** Complete combat loop - towers attack, enemies take damage, effects apply, win/lose conditions work.

**Tasks:**
1. Tower targeting system: scan enemies in range, pick target (furthest along path, matching original logic)
2. Implement attack subtypes:
   - **Cutter:** Instant melee `Slash`/`DeepSlash` - damage applies immediately to target(s) in range
   - **Bomber:** `ThrowableAttack` projectile - spawn projectile entity, move toward target, apply damage + area splash on contact
   - **Saucer:** `ThrowableAttack` projectile with slow effect on hit
3. Damage system: `DamageEvent` that reduces HP, checks for death
4. Death system: dango enters DYING state, plays death animation (placeholder), despawns, awards sugar
5. Effect system as components on dango entities:
   - `SlowEffect { timer: Timer, slow_percent: f32 }`
   - `BurnEffect { timer: Timer, damage_per_tick: f32 }`
   - `DeepWoundEffect { timer: Timer, damage_modifier: f32 }`
   - Systems tick these down, apply modifiers, remove on expiry
6. Saucer-specific: apply slow on hit
7. Cutter-specific: multi-target at higher levels, deep wound at max level
8. Bomber-specific: water bomb area damage at max level
9. Prospective damage tracking: prevent multiple towers from overkilling a target
10. Tower XP gain on enemy kill
11. Enrage / limit burst mode per tower type
12. Win condition: all waves cleared + all enemies dead
13. Lose condition: life reaches 0

**Deliverable:** Full combat loop works: place towers, start waves, towers auto-attack, enemies die, sugar earned, effects visible. Win and lose screens trigger correctly.

**Tests:**
- Unit: Damage formulas match config values; effect duration and stacking; prospective damage prevents overkill
- Integration: Place one bomber, spawn one dangosimple, verify it dies after expected number of hits
- **Golden test:** Load level 1, place specific towers at specific cells, run all waves, assert final sugar/life/XP values match expected outcome
- Manual: Combat feels correct, tower priorities work, no stuck states

---

### Phase 6: Spine Animations (Week 11-13)

**Goal:** Replace all static tower/enemy sprites with Spine skeleton animations.

**Tasks:**
1. Integrate `bevy_spine`: verify one skeleton loads and animates correctly
2. For each tower (cutter, saucer, bomber):
   - Load skeleton + atlas, replace static sprite with `SpineBundle`
   - Map tower states to animation names:
     - IDLE -> idle animation
     - ATTACKING -> attack animation (directional: right, up, down, left)
     - RELOADING -> idle or reload animation
     - LIMIT_BURSTING -> enrage animation
3. For each dango (dangosimple, dangobese, dangorille):
   - MOVE -> walk animation (directional)
   - ATTACK -> attack animation
   - DYING -> death animation
4. Animation direction updates: flip/set animation track based on movement direction
5. Animation speed scaling for game speed multiplier
6. Spine animation events: "attack frame reached" -> trigger damage application

**Fallback plan:** If `bevy_spine` proves inadequate after 2 weeks of effort, pivot to exporting Spine animations as spritesheet sequences using Spine's official export tool. Implement frame-based `TextureAtlas` animation system instead.

**Deliverable:** All towers and enemies animate smoothly with correct state transitions and directional facing.

**Tests:**
- Visual: Side-by-side comparison with original game
- Manual: Every animation plays, no glitches on state transitions
- Performance: 30+ enemies on screen with animations, maintain >55 FPS

---

### Phase 7: Walls and Special Attacks (Week 13-14)

**Goal:** Destructible walls, Dangobese ground pound, Dangorille monkey summon.

**Tasks:**
1. `Wall` component: HP, max HP, damage states, visual updates (sprite swaps at HP thresholds)
2. Spawn walls from level config; walls block path segments
3. Dango ATTACK state: dango reaches wall, attacks it, wall takes damage, wall destroyed -> dango continues along path
4. Dangobese special attack: area-of-effect ground pound
5. Dangorille special attack: summons `Monkey` entities that run to nearest tower
6. Tower BLOCKED state: when a Monkey reaches a tower, tower cannot attack until monkey is dealt with
7. `SpecialAttackAnnouncement` UI overlay

**Deliverable:** Walls appear, take damage, break. Boss enemies use special abilities. Monkeys interact with towers.

**Tests:**
- Unit: Wall HP deduction; monkey pathfinding to nearest tower
- Integration: Dangorille spawns monkeys, monkey blocks tower, tower unblocks when monkey removed

---

### Phase 8: UI Polish - Dialogues, Tutorials, Menus (Week 14-17)

**Goal:** All non-gameplay UI systems ported.

**Tasks:**
1. **Dialogue system:** JSON-driven dialogue sequences with character portraits, speech bubbles, progressive text reveal, character emotions (normal/angry), left/right positioning. Implement as `DialoguePlugin` with its own state machine.
2. **Tutorial system:** Port `TutorialFactory` with all 10 tutorial types:
   - TowerPositioningTutorial
   - SugarTutorial
   - LifeTutorial
   - UpgradeTutorial
   - SaucerTutorial
   - DangobeseTutorial
   - DangorillaTutorial
   - MultiPathsTutorial
   - DialogueTutorial
   Each tutorial checks conditions and displays instructional prompts.
3. **WinMenu:** Star display, challenge results, reward animations, next level button
4. **LoseMenu:** Advice display from config, retry button
5. **LevelParametersMenu** (pause): Resume, restart, quit, audio settings sliders
6. **SkillTree scene:** Talent tree display, buy/validate flow, requirement descriptions, `LeftPanel`, `BuyLayout`, `ValidationLayout`
7. **Shop scene:** In-game shop UI
8. **ChallengeHandler:** Per-level challenges (Untouchable, Perfectionist, Misanthrope, etc.) with runtime tracking and results
9. **LevelEditor scene** (lower priority, can be deferred): Consider using `bevy_egui` for editor-specific UI

**Deliverable:** Complete UI flow from first launch through menu, pre-level dialogues, in-game tutorials, win/lose screens, skill tree, shop.

**Tests:**
- Unit: Dialogue text progression, challenge condition evaluation
- Integration: Full playthrough of level 1 with all tutorials and dialogues
- Manual: All menus navigable, text readable, no overlap or clipping

---

### Phase 9: Persistence, Networking, and Save System (Week 17-19)

**Goal:** Game state persists between sessions. Server communication works.

**Tasks:**
1. Save system: serialize game state to JSON using `serde`:
   - Unlocked levels and world progress
   - Tower availability and XP
   - Sugar and holy sugar balance
   - Tutorial completion flags
   - Settings (audio volume, etc.)
   - Write to platform-appropriate path using `dirs` crate
2. Save loading at startup with fallback to defaults
3. Port `GameSettings` (difficulty, preferences) and `TutorialSettings` (seen tutorials)
4. Port `NetworkController`:
   - Create user account, upload tracking/progression, download new levels
   - Use `reqwest` with `bevy::tasks::AsyncComputeTaskPool` for async HTTP
   - Handle offline mode gracefully
5. Tracking system: scene transition events, level completion analytics

**Deliverable:** Progress persists across game launches. New levels downloadable from server (when online).

**Tests:**
- Unit: Serialization round-trip for all save data types
- Integration: Complete level, quit, relaunch, verify progress preserved
- Network: Mock HTTP server for integration tests

---

### Phase 10: Polish, Performance, and Platform Readiness (Week 19-22)

**Goal:** Production-quality game on all target platforms.

**Tasks:**
1. **Performance profiling:** Target 60 FPS on minimum spec
   - Optimize ECS queries, reduce allocations, batch sprite draws
   - Profile with `tracy` or Bevy's built-in diagnostics
2. **Resolution handling:**
   - Original design resolution: 1280x720
   - Implement camera scaling with `Camera2d` + `ScalingMode` for different screen sizes
3. **Visual polish:**
   - Screen-shake effects
   - Particle effects (sugar collection, tower placement)
   - Smooth interpolation on all animations
4. **Game speed control:** 1x / 2x toggle
5. **Cross-platform validation:**
   - Windows: primary target, should work out of the box
   - Linux/macOS: Bevy supports natively, minimal issues expected
   - iOS (future): Xcode project setup, touch input adaptation
   - Android (future): NDK build, touch input, texture budget considerations
   - WASM (future): bonus target, Bevy has good WASM support
6. Loading/splash screen with progress bar
7. App icon, window title, metadata

**Deliverable:** Game runs smoothly, looks and plays like the original, ready for distribution.

**Tests:**
- Performance: Benchmark with max enemies + towers, verify >55 FPS consistently
- Platform: Smoke test on each target platform
- Regression: Full playthrough of all levels

---

## 5. Risk Register

### Risk 1: Spine Animation Runtime - SEVERITY: HIGH

**Description:** `bevy_spine` may not support the exact Spine features used (animation mixing, events, skins) or may have version incompatibilities with the Spine data format version in this project.

**Mitigation:**
- Phase 0 includes early Spine validation with an actual game asset
- Spritesheet export fallback is ready (Spine's official tool can batch-export)
- Time-box Spine integration to 2 weeks in Phase 6; if not working, switch to spritesheet approach

### Risk 2: OOP-to-ECS Transformation Bugs - SEVERITY: MEDIUM

**Description:** The architecture transformation changes every interaction pattern. Subtle system ordering bugs, missing component updates, or entity lifetime issues could cause hard-to-find gameplay bugs.

**Mitigation:**
- Extensive integration tests in Phase 5 comparing outcomes to expected values from C++ code
- Use `bevy-inspector-egui` during development for real-time entity inspection
- Keep the original C++ build runnable throughout migration for A/B comparison

### Risk 3: UI Complexity with bevy_ui - SEVERITY: MEDIUM

**Description:** `bevy_ui` is less mature than cocos2d's UI widgets. Complex layouts (SkillTree, LevelEditor, dialogue system) may be painful.

**Mitigation:**
- Consider `bevy_egui` for development tools (LevelEditor)
- Invest early in reusable UI widget library (buttons, panels, sliders) in Phase 1
- SkillTree and Shop deferred to Phase 8 when UI patterns are well established

### Risk 4: Mobile Platform Support - SEVERITY: MEDIUM

**Description:** Bevy's iOS/Android support is functional but less battle-tested than desktop. Touch input, audio, file I/O may have edge cases.

**Mitigation:**
- Test on a real device early (during Phase 2) to catch platform issues before deep investment
- Use Bevy's cross-platform abstractions exclusively
- Budget extra time for mobile-specific issues

### Risk 5: Async Networking - SEVERITY: LOW

**Description:** Bevy's ECS is single-threaded for system execution. Async HTTP must be bridged carefully.

**Mitigation:** Use `bevy::tasks::AsyncComputeTaskPool::get().spawn()` with a channel or `Task` that the main thread polls. Well-documented Bevy pattern.

---

## 6. Testing Strategy

### Per-Phase Testing Matrix

| Phase | Unit Tests | Integration Tests | Manual Tests |
|---|---|---|---|
| 0 - Scaffold | JSON deserialization, config validation | App launches without panic | Window renders sprite |
| 1 - Menus | State transition logic | Navigate all menu paths | Music plays and fades |
| 2 - Grid | Grid math, path parsing | Level loads all elements | Visual comparison with original |
| 3 - Towers | Tower stats, sugar math | Place/upgrade/sell flow | Drag-and-drop UX |
| 4 - Enemies | Wave timing, path movement | Enemy full lifecycle | Smooth movement at 60 FPS |
| 5 - Combat | Damage, effects, targeting | Full combat scenarios | Win/lose conditions |
| 6 - Spine | Animation state mapping | All animations play | Visual fidelity vs original |
| 7 - Walls | Wall HP, special attacks | Boss enemy full behavior | Edge cases |
| 8 - UI | Dialogue parsing, tutorials | Full level playthrough | All UI flows |
| 9 - Save | Save serialization | Persistence round-trip | Cross-session save |
| 10 - Polish | N/A | Platform smoke tests | Full regression |

### Golden Test (from Phase 5 onward)

Maintain a deterministic integration test: load level 1, place specific towers at specific cells, run all waves with fixed random seed, assert final sugar/life/XP values. This catches gameplay regressions in any subsequent phase.

---

## 7. Key Crate Dependencies

| Purpose | Crate | Notes |
|---|---|---|
| Game engine | `bevy` 0.15 | Core framework |
| Spine animations | `bevy_spine` | Validate in Phase 0 |
| Serialization | `serde`, `serde_json` | Config + save files |
| Tweening/animation | `bevy_tweening` | Scene transitions, UI animations |
| Dev inspector | `bevy-inspector-egui` | Debug builds only |
| Editor UI | `bevy_egui` | For LevelEditor scene |
| HTTP networking | `reqwest` | Async HTTP client |
| File paths | `dirs` | Platform-appropriate save paths |
| Audio (optional) | `bevy_kira_audio` | If `bevy_audio` lacks needed features |

---

## 8. Critical Source Files Reference

These are the most important files in the original codebase that drive the migration:

| File | Role | Migration Impact |
|---|---|---|
| `Classes/Scenes/Level/Level.h/.cpp` | Core gameplay orchestrator | Decomposes into 10+ Bevy systems |
| `Classes/Scenes/Level/Towers/Tower.h/.cpp` | Tower base class + state machine | Becomes component composition + systems |
| `Classes/Scenes/Level/Dangos/Dango.h/.cpp` | Enemy base + path following + effects | Most complex entity decomposition |
| `Classes/Scenes/Level/Interface/LevelInterface.h/.cpp` | All gameplay UI + input handling | Rebuild in `bevy_ui` |
| `Classes/Config/Config.h/.cpp` | Central config + save manager | Becomes typed `Resource` structs |
| `Classes/SceneManager.h/.cpp` | Scene transitions + music | Becomes Bevy `States` |
| `Resources/res/config_towers.json` | Tower stats schema | Drives Rust struct definitions |
| `Resources/res/config_dangos.json` | Enemy stats schema | Drives Rust struct definitions |
| `Resources/res/config_levels.json` | Level definitions | Drives level loading system |

---

## 9. Phase Dependency Graph

```
Phase 0 (Scaffold)
    |
Phase 1 (Menus + Audio)
    |
Phase 2 (Grid + Paths)
    |
Phase 3 (Towers)----\
    |                 \
Phase 4 (Enemies)     |
    |                 |
Phase 5 (Combat) ----/
    |
    |--- Phase 6 (Spine Animations)  [can partially overlap with Phase 7]
    |
    |--- Phase 7 (Walls + Specials)  [can partially overlap with Phase 6]
    |
Phase 8 (UI Polish)  [SkillTree/Shop can start during Phase 6-7]
    |
Phase 9 (Save/Network) [can start during Phase 8]
    |
Phase 10 (Polish + Platforms)
```

Phases 6 and 7 can be worked in parallel by different developers. Phase 8's menu screens (SkillTree, Shop) can begin as soon as Phase 1 UI patterns are established. Phase 9 can overlap with Phase 8.
