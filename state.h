#ifndef STATE_H
#define STATE_H

#include "grid.h"

#include <SFML/Graphics.hpp>

struct Game;

enum States { INTRO_STATE, GAMEPLAY_STATE };

struct IntroState {
    States state_id = INTRO_STATE;
    sf::Time ticks;
};

struct GamePlayState {
    States state_id = GAMEPLAY_STATE;
    Grid* grid;
};

void intro_state_init(IntroState* state, Game* game);
void intro_state_handle_input(Game* game, const sf::Event& event);
void intro_state_update(IntroState* state, sf::Time dt);
void intro_state_render(IntroState* state, Game* game);

void game_state_init(GamePlayState* state, Game* game);
void game_state_deinit(GamePlayState* state);
void game_state_handle_input(Game* game, const sf::Event& event);
void game_state_update(GamePlayState* state, sf::Time dt);
void game_state_render(GamePlayState* state, Game* game);

union State {
    States state_id;
    IntroState intro;
    GamePlayState game_play;
};


#endif
