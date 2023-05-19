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
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "./math.h"
#include "./utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"


static int audio_open   = 0;
static Mix_Music* music = NULL;
static int next_track   = 0;
const int UPDATE_RATE = 1000 / 120; // update rate = 8.33 ms per frame.


const char* text         = "Press Any Key to Play";
const int default_ptsize = 36;

void opengl_debug_callback(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* user_params) {
    SDL_Log("OpenGL callback: %s", message);
}


enum AttribId { attrib_position, attrib_color };

struct Game {
    SDL_Window* window;
    int win_width, win_height;
    SDL_GLContext gl_context;
    std::filesystem::path assets_dir;
    std::unordered_map<const char*, GLuint> textures;
};

GameError init_game(Game* game,
                    const char* assets_dir,
                    const char* win_title,
                    int win_width,
                    int win_height) {

    if (!std::filesystem::exists(assets_dir)) {
        SDL_Log("Failed to find assets directory.\n");
        return GAME_ERROR_ASSETS_DIR_DOES_NOT_EXIST;
    }

    game->assets_dir = std::filesystem::path(assets_dir);

    SDL_Log("Assets directory path set to %s.\n",
            game->assets_dir.c_str());


    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("Failed to initialize SDL: %s\n", SDL_GetError());
        return GAME_ERROR_SDL_INIT_FAILED;
    }

    SDL_Log("SDL video and audio subsystems initialized "
            "successfully.\n");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);


    SDL_Window* window =
        SDL_CreateWindow(win_title,
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         win_width,
                         win_height,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL) {
        SDL_Log("Failed to create a window!: %s\n", SDL_GetError());
        SDL_Quit();
        return GAME_ERROR_WINDOW_CREATION_FAILED;
    }


    game->window     = window;
    game->win_width  = win_width;
    game->win_height = win_height;

    SDL_Log("A %dx%d window with title %s created "
            "successfully.\n",
            game->win_width,
            game->win_height,
            win_title);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        SDL_Log("Failed to create OpenGL context: %s\n",
                SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return GAME_ERROR_OPENGL_CONTEXT_CREATION_FAILED;
    }

    game->gl_context = context;


    glDebugMessageCallback(&opengl_debug_callback, 0);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);


    SDL_Log("OpenGL context created successfully: %p\n", game->gl_context);

    int audio_rate      = MIX_DEFAULT_FREQUENCY;
    int audio_channels  = MIX_DEFAULT_CHANNELS;
    Uint16 audio_format = MIX_DEFAULT_FORMAT;
    int audio_buffers   = 4096;


    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) <
        0) {
        SDL_Log("Failed to open audio device: %s\n", SDL_GetError());
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return GAME_ERROR_FAILED_TO_OPEN_AUDIO_DEVICE;
    }

    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

    SDL_Log("Opened audio device at %d Hz, %d bits%s %s "
            "%d bytes buffer.\n",
            audio_rate,
            audio_format & 0xFF,
            SDL_AUDIO_ISFLOAT(audio_format) ?
                " (float) " :
                "",
            audio_channels > 2     ? "surround" :
                audio_channels > 1 ? "mono" :
                                     "streo",
            audio_buffers);

    SDL_Log("Game initialized successfully.\n");


    return GAME_ERROR_NO_ERROR;
}

void unload_textures(Game* game) {
    for (const auto& p : game->textures) {
        glDeleteTextures(1, &p.second);
        SDL_Log("Unloaded texture \'%s\' with ID %d.\n", p.first, p.second);
    }
}

void quit_game(Game* game) {
    SDL_Log("Exitting game...");

    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(200);
    }

    // TODO: properly free the music of the "final" state.
    //
    //

    if (music) {
        Mix_FreeMusic(music);
    }

    Mix_CloseAudio();


    SDL_Log("Closed audio device.\n");

    SDL_Log("Unloading textures...\n");

    unload_textures(game);

    SDL_GL_DeleteContext(game->gl_context);

    SDL_Log("Deleted OpenGL context.\n");

    SDL_DestroyWindow(game->window);

    SDL_Log("Detroyed game window.\n");

    SDL_Quit();

    SDL_Log("Quited SDL.\n");
}

static const GLubyte tex_checkerboard_data[] = {
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
};

GameError load_textures(Game* game) {

    std::vector<std::pair<std::filesystem::path, const char*>> files_tags = {
        { game->assets_dir / "bg-v1.png", "bg" }
    };

    for (const auto& pair : files_tags) {
        if (!std::filesystem::exists(pair.first)) {
            SDL_Log("Failed to find asset %s in the "
                    "filesystem!",
                    pair.first.c_str());
            return GAME_ERROR_FILE_NOT_FOUND;
        }
    }


    for (int i = 0; i < files_tags.size(); i++) {
        GLuint texi;
        glGenTextures(1, &texi);
        glBindTexture(GL_TEXTURE_2D, texi);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nr_channels;
        // stbi_set_flip_vertically_on_load(true);

        unsigned char* data =
            stbi_load(files_tags[i].first.c_str(), &width, &height, &nr_channels, 0);

        SDL_Log("Image[%d] stats: width: %d, height: %d, "
                "nr "
                "of channels: %d\n",
                i,
                width,
                height,
                nr_channels);

        if (data) {
            // Allocate space for texture texi on the GPU.
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

            // Copy application data to the GPU.
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            game->textures[files_tags[i].second] = texi;
        } else {
            SDL_Log("Failed to load texture...");
        }

        stbi_image_free(data);
    }

    return GAME_ERROR_NO_ERROR;
}


struct Renderer {
    std::unordered_map<const char*, GLuint> shaders;
    GLuint sprite_vao;
    GLuint sprite_vbo;
};

void add_shader(Renderer* renderer,
                std::pair<const char*, GLuint> program) {
    renderer->shaders.insert(program);
}

void init_renderer(Renderer* renderer) {

    // clang-format off
    float verts[] = {
        // bottom left triangle
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        // top right triangle
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };
    // clang-format on
    //

    glGenVertexArrays(1, &renderer->sprite_vao);
    glGenBuffers(1, &renderer->sprite_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->sprite_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);


    glBindVertexArray(renderer->sprite_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void render_sprite(Game* game,
                   const char* tex,
                   Renderer* renderer,
                   const char* shader,
                   Vec2 position,
                   Vec2 size,
                   float rotate,
                   Vec3 color) {
    glUseProgram(renderer->shaders[shader]);

    Mat4x4 model;

    identity(model);
    // print_mat4x4("Identity Matrix", model);

    // Translate...
    translate(model, position);
    // print_mat4x4("I X T", model);

    // Rotate
    translate(model, { 0.5f * size.x, 0.5f * size.y });
    rotatez(model, rotate);
    translate(model, { -0.5f * size.x, -0.5f * size.y });
    // print_mat4x4("(I X T) X R", model);

    // Scale
    scale(model, size);
    // print_mat4x4("((I X T) X R) X S", model);

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders[shader], "model"),
                       1,
                       GL_TRUE,
                       model[0]);

    const float greenish[] = { 0.03, 0.98f, 0.01f };

    glUniform3fv(glGetUniformLocation(renderer->shaders[shader], "sprite_color"),
                 1,
                 greenish);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, game->textures[tex]);

    glBindTextureUnit(0, game->textures[tex]);

    glBindVertexArray(renderer->sprite_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


struct Cell {
    Vec2 position;
};

Cell c0 = { { 1.5, 1.5 } };

enum AnimState {
    ANIM_STARTING,
    ANIM_RUNNING,
    ANIM_STOPPED
};

AnimState current_anim_state = ANIM_STARTING;


void update(Uint64 dt) {

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

    GLuint main_shader;

    err = create_shader_program(game.assets_dir / "shaders" / "main.vs.glsl",
                                game.assets_dir / "shaders" / "main.fs.glsl",
                                &main_shader);
    if (err != 0) {
        SDL_Log("Failed to create a shader program\n");
        quit_game(&game);
        return err;
    }

    add_shader(&renderer, std::make_pair("main", main_shader));
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

    print_mat4x4("Projection Matrix", projection_matrix);


    glUseProgram(renderer.shaders["sprite"]);

    glUniformMatrix4fv(glGetUniformLocation(renderer.shaders["sprite"],
                                            "projection"),
                       1,
                       GL_TRUE,
                       projection_matrix[0]);
    glUniform1i(
        glGetUniformLocation(renderer.shaders["sprite"], "image"), 0);


    const char* typ = NULL;


    music = Mix_LoadMUS_RW(
        SDL_RWFromFile((game.assets_dir / "bg.mp3").c_str(), "rb"),
        SDL_TRUE);

    switch (Mix_GetMusicType(music)) {
        case MUS_CMD: typ = "CMD"; break;
        case MUS_FLAC: typ = "FLAC"; break;
        case MUS_OGG: typ = "OGG"; break;
        default: typ = "NONE"; break;
    }

    SDL_Log("Detected music type %s\n", typ);

    Mix_FadeInMusic(music, -1, 2000);


    Uint64 prev_time = SDL_GetTicks64();
    Uint32 lag_time  = 0;

    bool game_running = true;

    while (game_running) {

        Uint64 current_time = SDL_GetTicks64();
        Uint64 dt           = current_time - prev_time;

        lag_time += dt;


        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    game_running = false;
                    break;
                default: break;
            }
        }

        while (lag_time >= UPDATE_RATE) {
            update(dt);
            lag_time -= UPDATE_RATE;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        render_sprite(&game,
                      "bg",
                      &renderer,
                      "sprite",
                      { 0, 0 },
                      { (float)game.win_width, (float)game.win_height },
                      0,
                      { 2, 2, 2 });

        SDL_GL_SwapWindow(game.window);


        prev_time = current_time;
    }

    quit_game(&game);

    return 0;
}
