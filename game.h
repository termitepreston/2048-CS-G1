#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "utils.h"

#include "SDL.h"
#define GL_GLEXT_PROTOTYPES
#include "SDL_mixer.h"
#include "SDL_opengl.h"


#include <filesystem>
#include <stack>
#include <unordered_map>


void opengl_debug_callback(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* user_params);

struct Game {
    SDL_Window* window;
    int win_width, win_height;
    SDL_GLContext gl_context;
    std::filesystem::path assets_dir;
    std::unordered_map<std::string, GLuint> textures;
    Mix_Music* music;
    std::stack<State> states;
};

GameError init_game(Game* game,
                    const char* assets_dir,
                    const char* win_title,
                    int win_width,
                    int win_height);
void unload_textures(Game* game);

void quit_game(Game* game);

GameError load_textures(Game* game);

#endif // !GAME_H
