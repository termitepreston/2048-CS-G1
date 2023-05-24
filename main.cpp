#include "anim.h"
#include "game.h"
#include "grid.h"
#include "math.h"
#include "state.h"
#include "utils.h"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
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
#include "state.h"
#include "utils.h"

#include <SFML/Graphics.hpp>


const int UPDATE_RATE = 1000 / 120; // update rate = 16.66 ms per frame.


enum AnimState {
    ANIM_STARTING,
    ANIM_RUNNING,
    ANIM_STOPPED
};

AnimState current_anim_state = ANIM_STARTING;

const float zeta  = 0.23;
const float omega = 3 * 3.141592653589793;


void update(sf::Uint64 dt, Cell* cell, Grid* grid) {

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


void usage(const char* program) {
    printf("usage: %s -assets [dir]\n", program);
}

using namespace std;


int main(int argc, char* argv[]) {

    // Parse command line arguments.
    int i;
    char assets_dir[100];
    char* argv0 = argv[0];

    for (i = 1; argv[i] && argv[i][0] == '-'; ++i) {
        if (strcmp(argv[i], "--assets") == 0) {
            strncpy(assets_dir, argv[++i], 100);
        } else {
            usage(argv0);
            return 1;
        }
    }


    Game game;

    GameError err =
        init_game(&game, assets_dir, "2048 - CS222 Edition", 480, 640);

    if (err != 0) {
        printf("Game init failed\n");
        return err;
    }

    err = load_textures(&game);

    if (err != 0) {
        printf("Assets loading failed...\n");
        return err;
    }


    IntroState intro;
    intro_state_init(&intro, &game);
    State state = { .intro = intro };

    game.states.push(state);


    sf::Time lag_time;
    sf::Clock clock;


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

    while (game.window.isOpen()) {

        sf::Time elapsed = clock.getElapsedTime();
        lag_time += elapsed;


        sf::Event event;
        while (game.window.pollEvent(event)) {
            switch (game.states.top().state_id) {
                case INTRO_STATE:
                    intro_state_handle_input(&game, event);
                    break;
                case GAMEPLAY_STATE: break;
                default: break;
            }
        }

        while (lag_time.asMilliseconds() >= UPDATE_RATE) {
            // update(dt, &grid.cells[0], &grid);
            switch (game.states.top().state_id) {
                case INTRO_STATE:
                    intro_state_update(&game.states.top().intro);
                    break;
                case GAMEPLAY_STATE: break;
                default: break;
            }
            lag_time -= sf::milliseconds(UPDATE_RATE);
        }

        game.window.clear();

        switch (game.states.top().state_id) {
            case INTRO_STATE:
                intro_state_render(&game.states.top().intro, &game);
                break;
            case GAMEPLAY_STATE: break;

            default: break;
        }

        game.window.display();
    }

    quit_game(&game);

    return 0;
}
