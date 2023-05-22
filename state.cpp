#include "state.h"
#include "SDL_timer.h"

#include "renderer.h"


void intro_state_init(IntroState* state) {
    state->ticks = 0.0f;
}

void intro_state_update(IntroState* state) {
    state->ticks = (float)SDL_GetTicks();
}

void intro_state_render(IntroState* state, Renderer* renderer) {
    // use program blinking sprite.
    // set the uniform 'time'.
    // render the textured quad...
}
