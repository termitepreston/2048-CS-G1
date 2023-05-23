#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_log.h>
#include <SDL_mixer.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include <SDL_rwops.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "anim.h"
#include "game.h"
#include "grid.h"
#include "math.h"
#include "renderer.h"
#include "state.h"
#include "utils.h"


const int UPDATE_RATE = 1000 / 120; // update rate = 16.66 ms per frame.


enum AttribId { attrib_position, attrib_color };


enum AnimState {
    ANIM_STARTING,
    ANIM_RUNNING,
    ANIM_STOPPED
};

AnimState current_anim_state = ANIM_STARTING;

const float zeta  = 0.23;
const float omega = 3 * 3.141592653589793;


void update(Uint64 dt, Cell* cell, Grid* grid) {

    /*
     * we have an animation fsm whose individual states are
     * animation states which are represented by animstate enum.
     *
     * the cells x, y position is function of the current animation state.
     *
     * we have three cases for animation state:
     *
     * case 0: the animation has not started:
     *
     * In this case, the cells internal state, that is, its position is not
     * update.
     *
     * case 2: animation is starting:
     *
     * set up animation control variables based on the following params:
     *
     * a) animation time.
     * b) animation start and end postions.
     *
     * set up current animation status variables:
     *
     * i) current position or normalized postion.
     *
     *
     *
     * case 1: the animation is running:
     *
     * A new state (postion) is calculated for the cell based animation start
     * position, animation stop position, animation time and dt.
     *
     * case 2: the animation has completed:
     *
     * Nothing is done to the cells internal state.
     *
     */

    spring(cell->position.x,
           cell->velocity.x,
           grid->cells[2].position.x,
           zeta,
           omega,
           (float)dt / 1000.0f);
}

void handle_input(const SDL_Event& event, AnimState* state) {
}

void usage(const char* program) {
    SDL_Log("usage: %s -assets [dir]\n", program);
}

using namespace std;


int main(int argc, char* argv[]) {

    // Parse command line arguments.
    int i;
    char assets_dir[100];
    char* argv0 = argv[0];

    for (i = 1; argv[i] && argv[i][0] == '-'; ++i) {
        if (SDL_strcmp(argv[i], "--assets") == 0) {
            SDL_strlcpy(assets_dir, argv[++i], 100);
        } else {
            usage(argv0);
            return 1;
        }
    }


    Game game;

    GameError err =
        init_game(&game, assets_dir, "2048 - CS222 Edition", 480, 640);

    if (err != 0) {
        SDL_Log("Game init failed\n");
        return err;
    }

    err = load_textures(&game);

    if (err != 0) {
        SDL_Log("Assets loading failed...\n");
        return err;
    }

    if (glIsTexture(game.textures.at("bg")) != GL_TRUE) {
        SDL_Log("Something has happned here...\n");
    }
    //
    // Create the renderer...
    //

    Renderer renderer;
    init_renderer(&renderer);

    // create and use sprite shader...
    //


    GLuint sprite_shader;
    err = create_shader_program(game.assets_dir / "shaders" / "sprite.vs.glsl",
                                game.assets_dir / "shaders" / "sprite.fs.glsl",
                                &sprite_shader);

    if (err != 0) {
        SDL_Log("Failed to create sprite shader.\n");
        quit_game(&game);
        return err;
    }

    GLuint blink_shader;
    err = create_shader_program(game.assets_dir / "shaders" / "blink.vs.glsl",
                                game.assets_dir / "shaders" / "blink.fs.glsl",
                                &blink_shader);
    if (err != 0) {
        SDL_Log(
            "Failed to create blink shader program\n");
        quit_game(&game);
        return err;
    }

    add_shader(&renderer, std::make_pair("blink", blink_shader));
    add_shader(&renderer, std::make_pair("sprite", sprite_shader));


    glDisable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.0, 0.0, 0.0);
    glViewport(0, 0, game.win_width, game.win_height);


    Mat4x4 projection_matrix;

    mat4x4_ortho(projection_matrix,
                 0.0f,
                 (float)game.win_width,
                 (float)game.win_height,
                 (float)0.0f,
                 0.0f,
                 100.0f);

    print_mat4x4("Projection Matrix - Sprite", projection_matrix);


    glUseProgram(renderer.shaders["sprite"]);

    glUniformMatrix4fv(glGetUniformLocation(renderer.shaders["sprite"],
                                            "projection"),
                       1,
                       GL_TRUE,
                       projection_matrix[0]);
    glUniform1i(
        glGetUniformLocation(renderer.shaders["sprite"], "image"), 0);


    IntroState intro;
    intro_state_init(&intro, &game, &renderer);
    State state = { .intro = intro };

    game.states.push(state);

    const char* typ = NULL;


    game.music = Mix_LoadMUS_RW(
        SDL_RWFromFile((game.assets_dir / "bg.mp3").c_str(), "rb"),
        SDL_TRUE);

    switch (Mix_GetMusicType(game.music)) {
        case MUS_CMD: typ = "CMD"; break;
        case MUS_FLAC: typ = "FLAC"; break;
        case MUS_OGG: typ = "OGG"; break;
        default: typ = "NONE"; break;
    }

    SDL_Log("Detected music type %s\n", typ);

    Mix_FadeInMusic(game.music, -1, 2000);


    Uint64 prev_time = SDL_GetTicks64();
    Uint32 lag_time  = 0;

    bool game_running = true;

    Grid grid;
    init_grid(&grid,
              {
                  50.0f,
                  50.0f,
              },
              4,
              5.0f,
              5.0f,
              5.0f,
              75.0f);

    while (game.running) {

        Uint64 current_time = SDL_GetTicks64();
        Uint64 dt           = current_time - prev_time;

        lag_time += dt;


        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (game.states.top().state_id) {
                case INTRO_STATE:
                    intro_state_handle_input(&game, &event);
                    break;
                case GAMEPLAY_STATE: break;
                default: break;
            }
        }

        while (lag_time >= UPDATE_RATE) {
            // update(dt, &grid.cells[0], &grid);
            switch (game.states.top().state_id) {
                case INTRO_STATE:
                    intro_state_update(&game.states.top().intro);
                    break;
                case GAMEPLAY_STATE: break;
                default: break;
            }
            lag_time -= UPDATE_RATE;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        switch (game.states.top().state_id) {
            case INTRO_STATE:
                intro_state_render(&game.states.top().intro, &game, &renderer);
                break;
            case GAMEPLAY_STATE:
                render_sprite(&game,
                              "bg",
                              &renderer,
                              "sprite",
                              { 0, 0 },
                              { (float)game.win_width,
                                (float)game.win_height },
                              0,
                              { 2, 2, 2 });
                render_sprite(&game,
                              "2048",
                              &renderer,
                              "sprite",
                              grid.cells[0].position,
                              grid.cells[0].size,
                              0,
                              { 0, 0, 0 });
                break;

            default: break;
        }
        SDL_GL_SwapWindow(game.window);


        prev_time = current_time;
    }

    quit_game(&game);

    return 0;
}
