#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_log.h>
#include <SDL_mixer.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <SDL_video.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <signal.h>


static int audio_open   = 0;
static Mix_Music* music = NULL;
static int next_track   = 0;


const char* text         = "Press Any Key to Play";
const int default_ptsize = 36;

enum TextRenderMethod {
    TextRenderSolid,
    TextRenderShaded,
    TextRenderBlended
};

struct Scene {
    SDL_Texture* caption;
    SDL_Rect caption_rect;
    SDL_Texture* message;
    SDL_Rect message_rect;
};

void cleanup_audio(int exitcode) {
    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(500);
    }

    if (music) {
        Mix_FreeMusic(music);
    }

    if (audio_open) {
        Mix_CloseAudio();
        audio_open = 0;
    }

    exit(exitcode);
}

void cleanup_ttf() {
    TTF_Quit();
}


int main(int argc, char* argv[]) {
    const int win_width = 480, win_height = 640;

    bool game_running = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 255;
    }

    SDL_Window* window =
        SDL_CreateWindow("2048 - CS222 Edition",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         win_width,
                         win_height,
                         0);

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 141, 177, 125, 255);

    SDL_Surface* surface =
        IMG_Load("../assets/bg-v1.png");
    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);


    SDL_Rect src  = { 0, 0, win_width, win_height };
    SDL_Rect dest = { 0, 0, win_width, win_height };

    // Audio related initialization code...

    int audio_rate      = MIX_DEFAULT_FREQUENCY;
    Uint16 audio_format = MIX_DEFAULT_FORMAT;
    int audio_channels  = MIX_DEFAULT_CHANNELS;
    int audio_buffers   = 4096;
    int audio_volume    = MIX_MAX_VOLUME;
    int looping         = 0;
    int rwops           = 0;
    int i;
    const char* typ = NULL;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) <
        0) {
        SDL_Log("Could not open audio device: %s\n", SDL_GetError());
        return 2;
    } else {
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

        SDL_Log("Opened audio at %d Hz %d bit%s %s %d "
                "bytes audio buffer\n",
                audio_rate,
                (audio_format & 0xFF),
                (SDL_AUDIO_ISFLOAT(audio_format) ?
                     " (float)" :
                     ""),
                (audio_channels > 2       ? "surround" :
                     (audio_channels > 1) ? "stereo" :
                                            "mono"),
                audio_buffers);
    }

    audio_open = 1;

    music =
        Mix_LoadMUS_RW(SDL_RWFromFile("../assets/bg.mp3", "rb"), SDL_TRUE);

    switch (Mix_GetMusicType(music)) {
        case MUS_CMD: typ = "CMD"; break;
        case MUS_FLAC: typ = "FLAC"; break;
        case MUS_OGG: typ = "OGG"; break;
        default: typ = "NONE"; break;
    }

    SDL_Log("Detected music type %s\n", typ);

    Mix_FadeInMusic(music, -1, 2000);

    TTF_Font* font;
    SDL_Surface* text = NULL;
    Scene scene;
    int ptsize;
    SDL_Color white      = { 0xFF, 0xFF, 0xFF, 0 },
              black      = { 0, 0, 0, 0 };
    SDL_Color *forecolor = &black, *backcolor = &white;
    TextRenderMethod render_method = TextRenderSolid;
    int render_sytle;
    int outline;
    int hinting;
    int kerning;
    int dump;

    enum RenderType {
        RENDER_LATIN1,
        RENDER_UTF8,
        RENDER_UNICODE
    } render_type;

    char *message, string[128] = "Font: Lobster Regular";

    if (TTF_Init() < 0) {
        SDL_Log("Could not initialize SDL TTF: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    font = TTF_OpenFont("../assets/Lobster-Regular.ttf", default_ptsize);

    if (font == NULL) {
        SDL_Log("Couldn't load %d pt font from the given "
                "file: %s\n",
                default_ptsize,
                SDL_GetError());
        cleanup_ttf();
    }

    TTF_SetFontStyle(font, render_sytle);
    TTF_SetFontOutline(font, 0);
    TTF_SetFontKerning(font, 1);
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);

    switch (render_method) {
        case TextRenderSolid:
            text = TTF_RenderText_Solid(font, string, *forecolor);
            break;
        case TextRenderShaded:
            text = TTF_RenderText_Shaded(font, string, *forecolor, *backcolor);
            break;
        case TextRenderBlended:
            text = TTF_RenderText_Blended(font, string, *forecolor);
            break;
    }

    if (text != NULL) {
        scene.caption_rect.x = 4;
        scene.caption_rect.y = 4;
        scene.caption_rect.w = text->w;
        scene.caption_rect.h = text->h;
        scene.caption = SDL_CreateTextureFromSurface(renderer, text);
        SDL_FreeSurface(text);
    }

    Uint64 prev_time = SDL_GetTicks64();


    while (game_running) {

        Uint64 current_time = SDL_GetTicks64();
        Uint64 dt           = current_time - prev_time;

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    game_running = false;
                    break;
                default: break;
            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, &src, &dest);

        SDL_RenderCopy(renderer,
                       scene.caption,
                       NULL,
                       &scene.caption_rect);

        SDL_RenderPresent(renderer);

        prev_time = current_time;
    }

    cleanup_audio(0);


    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(scene.caption);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
