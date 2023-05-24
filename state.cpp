#include "state.h"

#include "game.h"
#include "grid.h"
#include "math.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>


void intro_state_init(IntroState* state, Game* game) {
    state->ticks = sf::milliseconds(0);
}

void intro_state_handle_input(Game* game, const sf::Event& event) {
    switch (event.type) {
        case sf::Event::KeyReleased: {
            GamePlayState gameplay;
            game_state_init(&gameplay, game);

            game->states.pop();
            game->states.push({ .game_play = gameplay });
            break;
        }
        default: break;
    }
}

void intro_state_update(IntroState* state, sf::Time dt) {
    state->state_id = States::INTRO_STATE;
    state->ticks += dt;
}

void intro_state_render(IntroState* state, Game* game) {
    sf::Sprite bg;
    bg.setTexture(game->textures["bg"]);

    sf::Sprite press;
    press.setTexture(game->textures["press"]);


    // center it in the middle of the screen.
    press.setPosition(sf::Vector2f(
        ((float)game->win_width / 2) -
            (float)press.getTexture()->getSize().x / 2,
        490));

    // blinking sprite...
    const float freq = 0.0025;
    press.setColor(sf::Color(
        255,
        255,
        255,
        255 * std::abs(std::sin(freq * state->ticks.asMilliseconds()))));

    game->window.draw(bg);
    game->window.draw(press);
}

void game_state_init(GamePlayState* state, Game* game) {
    int grid_sz   = 4;
    float cell_sz = 80.0f;
    float gutter  = 8.0f;


    state->grid = new Grid;

    float extent = calc_extent(gutter, cell_sz, grid_sz);

    printf("extent: %f\n", extent);

    sf::Vector2f pos = {
        ((float)game->win_width / 2.0f) - (extent / 2.0f), 60.0f
    };

    init_grid(state->grid, pos, grid_sz, gutter, cell_sz);


    printf("state->grid->cells.size() = %ld\n",
           state->grid->cells.size());
}

void game_state_deinit(GamePlayState* state) {
    delete state->grid;
}

void game_state_handle_input(Game* game, const sf::Event& event) {
}
void game_state_update(GamePlayState* state, sf::Time dt) {
}

void game_state_render(GamePlayState* state, Game* game) {
    // render background
    //
    sf::RectangleShape bg(sf::Vector2f(game->win_width, game->win_height));
    bg.setFillColor(sf::Color(250, 248, 239, 255));
    game->window.draw(bg);
    // render the grid;
    //
    float extent = calc_extent(8.0f, 80.0f, 4);


    sf::RectangleShape rect(sf::Vector2f(extent, extent));

    rect.setFillColor(sf::Color(187, 173, 160, 255));
    rect.setPosition(state->grid->position);

    game->window.draw(rect);


    sf::RectangleShape cell;
    for (int i = 0; i < state->grid->cells.size(); i++) {
        cell.setSize(state->grid->cells[i].size);
        cell.setPosition(state->grid->cells[i].position);
        cell.setFillColor(sf::Color(205, 193, 180, 255));
        game->window.draw(cell);
    }
}
