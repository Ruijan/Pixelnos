use bevy::prelude::*;

/// Design resolution used for coordinate mapping.
const DESIGN_WIDTH: f32 = 1280.0;
const DESIGN_HEIGHT: f32 = 720.0;

/// Number of cells across the grid width.
pub const GRID_COLS: usize = 16;
/// Number of cells tall.
pub const GRID_ROWS: usize = 12;

/// Resource holding the runtime grid state.
#[derive(Resource)]
pub struct Grid {
    /// Cell size in world pixels.
    pub cell_size: f32,
    /// Grid origin (bottom-left corner) in world space.
    pub origin: Vec2,
    /// Per-cell data: `cells[col][row]`.
    pub cells: Vec<Vec<CellState>>,
}

/// State of a single grid cell.
#[derive(Clone, Debug, Default)]
pub struct CellState {
    /// Whether this cell is part of an enemy path.
    pub is_path: bool,
    /// Whether this cell is off-limits (not buildable).
    pub off_limit: bool,
    /// Entity of the tower occupying this cell, if any.
    pub occupant: Option<Entity>,
}

impl CellState {
    pub fn is_free(&self) -> bool {
        !self.is_path && !self.off_limit && self.occupant.is_none()
    }
}

impl Grid {
    /// Create a new grid sized to the current window.
    pub fn new(window_width: f32, window_height: f32) -> Self {
        // Cell size calculation matching the original C++:
        //   size_cell = (3/4) * width * sqrt(min_ratio / aspect) / 12
        // Simplified for our fixed 16x12 grid:
        let aspect = window_width / window_height;
        let min_ratio = (DESIGN_WIDTH / DESIGN_HEIGHT).min(aspect);
        let cell_size = (0.75 * window_width * (min_ratio / aspect).sqrt()) / 12.0;

        // Grid is centered on screen. Origin = bottom-left of grid.
        let grid_width = GRID_COLS as f32 * cell_size;
        let grid_height = GRID_ROWS as f32 * cell_size;
        let origin = Vec2::new(-grid_width / 2.0, -grid_height / 2.0);

        let cells = vec![vec![CellState::default(); GRID_ROWS]; GRID_COLS];

        Self {
            cell_size,
            origin,
            cells,
        }
    }

    /// Mark cells along a path as path cells.
    pub fn mark_path(&mut self, waypoints: &[[f64; 2]]) {
        for wp in waypoints {
            if let Some((col, row)) = self.world_to_grid(Vec2::new(wp[0] as f32, wp[1] as f32)) {
                self.cells[col][row].is_path = true;
            }
        }
    }

    /// Convert world position to grid (col, row). Returns None if out of bounds.
    pub fn world_to_grid(&self, world_pos: Vec2) -> Option<(usize, usize)> {
        let local = world_pos - self.origin;
        let col = (local.x / self.cell_size).floor() as i32;
        let row = (local.y / self.cell_size).floor() as i32;
        if col >= 0 && col < GRID_COLS as i32 && row >= 0 && row < GRID_ROWS as i32 {
            Some((col as usize, row as usize))
        } else {
            None
        }
    }

    /// Convert grid (col, row) to world center position.
    pub fn grid_to_world(&self, col: usize, row: usize) -> Vec2 {
        Vec2::new(
            self.origin.x + (col as f32 + 0.5) * self.cell_size,
            self.origin.y + (row as f32 + 0.5) * self.cell_size,
        )
    }

    /// Find the nearest valid (free) cell to a world position.
    pub fn nearest_free_cell(&self, world_pos: Vec2) -> Option<(usize, usize)> {
        let (col, row) = self.world_to_grid(world_pos)?;
        if self.cells[col][row].is_free() {
            return Some((col, row));
        }
        // Search nearby cells
        for radius in 1..4i32 {
            for dc in -radius..=radius {
                for dr in -radius..=radius {
                    let c = col as i32 + dc;
                    let r = row as i32 + dr;
                    if c >= 0 && c < GRID_COLS as i32 && r >= 0 && r < GRID_ROWS as i32 {
                        let (uc, ur) = (c as usize, r as usize);
                        if self.cells[uc][ur].is_free() {
                            return Some((uc, ur));
                        }
                    }
                }
            }
        }
        None
    }
}
