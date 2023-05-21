#ifndef GRID_H
#define GRID_H

#include "math.h"
#include <vector>

struct Cell {
    int val;
    Vec2 position;
    Vec2 velocity;
    Vec2 size;
    float rotation;
};

struct Grid {
    Vec2 position;
    int grid_sz;
    int cell_sz;
    float mx, my, side, gutter;
    std::vector<Cell> cells;
};

void init_grid(Grid* grid,
               Vec2 position,
               int grid_sz,
               float gutter,
               float mx,
               float my,
               float cell_sz);

#endif
