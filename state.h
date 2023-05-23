#ifndef STATE_H
#define STATE_H


#include "SDL.h"


struct Renderer;
struct Game;

enum States { INTRO_STATE, GAMEPLAY_STATE };

struct IntroState {
    States state_id = INTRO_STATE;
    float ticks;
};

void intro_state_init(IntroState* state, Game* game, Renderer* renderer);
void intro_state_handle_input(Game* game, SDL_Event* event);
void intro_state_update(IntroState* state);
void intro_state_render(IntroState* state, Game* game, Renderer* renderer);

struct GamePlayState {};


union State {
    States state_id;
    IntroState intro;
    GamePlayState game_play;
};


#endif
