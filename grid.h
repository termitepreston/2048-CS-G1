#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>

#include <vector>

struct Cell {
    int val;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f size;
    float rotation;
};

struct Grid {
    sf::Vector2f position;
    int grid_sz;
    int cell_sz;
    float side, gutter;
    std::vector<Cell> cells;
};

float calc_extent(float gutter, float cell_sz, int grid_sz);
void init_grid(Grid* grid, sf::Vector2f position, int grid_sz, float gutter, float cell_sz);

#endif
