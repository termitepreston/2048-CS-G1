#include "grid.h"

void init_grid(Grid* grid,
               Vec2 position,
               int grid_sz,
               float gutter,
               float mx,
               float my,
               float cell_sz) {
    grid->position.x = position.x;
    grid->position.y = position.y;
    grid->grid_sz    = grid_sz;
    grid->cell_sz    = cell_sz;
    grid->gutter     = gutter;
    grid->mx         = mx;
    grid->my         = my;

    grid->cells.reserve(grid->grid_sz * grid->grid_sz);

    for (int row = 0; row < grid->grid_sz; row++) {
        for (int col = 0; col < grid->grid_sz; col++) {
            grid->cells[row * 4 + col] = { 0,
                                           { 0.0f, 0.0f },
                                           { 0.0f, 0.0f },
                                           { (float)cell_sz, (float)cell_sz },
                                           0.0f };

            grid->cells[row * 4 + col].position.x =
                grid->position.x + mx +
                col * (grid->cell_sz + grid->gutter);
            grid->cells[row * 4 + col].position.y =
                grid->position.y + my +
                col * (grid->cell_sz + grid->gutter);
        }
    }
}
