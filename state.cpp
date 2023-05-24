#include "state.h"

#include "math.h"


void intro_state_init(IntroState* state, Game* game) {
    state->ticks = 0.0f;
}

void intro_state_handle_input(Game* game, const sf::Event& event) {
    switch (event.type) {
        default: break;
    }
}

void intro_state_update(IntroState* state) {
    state->state_id = States::INTRO_STATE;
    state->ticks    = 0;
}

void intro_state_render(IntroState* state, Game* game) {
}
