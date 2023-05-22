#ifndef STATE_H
#define STATE_H

struct Renderer;

enum States { INTRO_STATE, GAMEPLAY_STATE };

struct IntroState {
    States state_id = INTRO_STATE;
    float ticks;
};

void intro_state_init(IntroState* state);
void intro_state_update(IntroState* state);
void intor_state_render(IntroState* state, Renderer* renderer);

struct GamePlayState {};


union State {
    States state_id;
    IntroState intro;
    GamePlayState game_play;
};


#endif
