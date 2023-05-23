#include "game.h"
#include "state.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"


#define STB_TRUETYPE_IMPLEMENTATION
#include "libs/stb_truetype.h"
void opengl_debug_callback(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* user_params) {
    SDL_Log("OpenGL callback: %s", message);
}

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

    game->running = true;

    SDL_Log("Game initialized successfully.\n");


    return GAME_ERROR_NO_ERROR;
}


void unload_textures(Game* game) {
    for (const auto& p : game->textures) {
        glDeleteTextures(1, &p.second);
        SDL_Log("Unloaded texture \'%s\' with ID %d.\n",
                p.first.c_str(),
                p.second);
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

    if (game->music) {
        Mix_FreeMusic(game->music);
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

GameError load_textures(Game* game) {

    std::vector<std::pair<std::filesystem::path, const char*>> files_tags = {
        { game->assets_dir / "bg-v1.png", "bg" },
        { game->assets_dir / "2.png", "2" },
        { game->assets_dir / "4.png", "4" },
        { game->assets_dir / "8.png", "8" },
        { game->assets_dir / "16.png", "16" },
        { game->assets_dir / "32.png", "32" },
        { game->assets_dir / "64.png", "64" },
        { game->assets_dir / "128.png", "128" },
        { game->assets_dir / "256.png", "256" },
        { game->assets_dir / "512.png", "512" },
        { game->assets_dir / "1024.png", "1024" },
        { game->assets_dir / "2048.png", "2048" },
        { game->assets_dir / "press.png", "press" }
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


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nr_channels;
        // stbi_set_flip_vertically_on_load(true);

        unsigned char* data =
            stbi_load(files_tags[i].first.c_str(), &width, &height, &nr_channels, 0);

        SDL_Log("Image[%i] \'%s\' stats: width: %d, "
                "height: %d, "
                "nr of channels: %d, OpenGL handle: %d\n",
                i,
                files_tags[i].second,
                width,
                height,
                nr_channels,
                texi);

        if (data) {
            // Allocate space for texture texi on the GPU.
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

            // Copy application data to the GPU.
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            game->textures.insert({ files_tags[i].second, texi });

        } else {
            SDL_Log("Failed to load texture...");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }

    return GAME_ERROR_NO_ERROR;
}
