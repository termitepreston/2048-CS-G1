#include "grid.h"

void init_grid(Grid* grid, sf::Vector2f position, int grid_sz, float gutter, float cell_sz) {
    grid->position.x = position.x;
    grid->position.y = position.y;
    grid->grid_sz    = grid_sz;
    grid->cell_sz    = cell_sz;
    grid->gutter     = gutter;

    grid->cells.reserve(grid->grid_sz * grid->grid_sz);

    printf("grid->cells.size() = %ld\n", grid->cells.size());
    printf("grid->cells.cap() = %ld\n", grid->cells.capacity());

    for (int row = 0; row < grid->grid_sz; row++) {
        for (int col = 0; col < grid->grid_sz; col++) {
            grid->cells.push_back(
                { 0,
                  sf::Vector2f(grid->position.x + gutter +
                                   col * (grid->cell_sz + grid->gutter),
                               grid->position.y + gutter +
                                   row * (grid->cell_sz + grid->gutter)),
                  { 0.0f, 0.0f },
                  { (float)cell_sz, (float)cell_sz },
                  0.0f });
        }
    }
}

float calc_extent(float gutter, float cell_sz, int grid_sz) {
    return (grid_sz * cell_sz) + ((1 + grid_sz) * gutter);
}
