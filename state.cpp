#include "state.h"
#include "SDL_events.h"
#include "SDL_timer.h"

#include "math.h"
#include "renderer.h"


void intro_state_init(IntroState* state, Game* game, Renderer* renderer) {
    state->ticks = 0.0f;

    Mat4x4 projection;

    mat4x4_ortho(projection,
                 0.0f,
                 (float)game->win_width,
                 (float)game->win_height,
                 (float)0.0f,
                 0.0f,
                 100.0f);
    // setup projection matrix...
    use_shader(renderer, "blink");

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders["blink"],
                                            "projection"),
                       1,
                       GL_TRUE,
                       projection[0]);
    glUniform1i(
        glGetUniformLocation(renderer->shaders["blink"], "image"), 0);
}

void intro_state_handle_input(Game* game, SDL_Event* event) {
    switch ((*event).type) {
        case SDL_QUIT: game->running = false; break;

        case SDL_KEYUP:
            SDL_Log("Key pressed up!\n");
            break;

        default: break;
    }
}

void intro_state_update(IntroState* state) {
    state->state_id = States::INTRO_STATE;
    state->ticks    = (float)SDL_GetTicks();
}

void intro_state_render(IntroState* state, Game* game, Renderer* renderer) {
    // use program blinking sprite.
    use_shader(renderer, "sprite");

    glUniform1f(glGetUniformLocation(
                    renderer->shaders["sprite"], "time"),
                (float)state->ticks);

    // render the textured quad...
    render_sprite(game,
                  "bg",
                  renderer,
                  "sprite",
                  { 0.0f, 0.0f },
                  { (float)game->win_width, (float)game->win_height },
                  0,
                  { 0, 0, 0 });

    use_shader(renderer, "blink");


    // set the uniform 'time'.
    glUniform1f(
        glGetUniformLocation(renderer->shaders["blink"], "time"),
        state->ticks);

    render_sprite(game,
                  "press",
                  renderer,
                  "blink",
                  { 50, 400 },
                  { 418, 133 },
                  0,
                  { 0, 0, 0 });
}
