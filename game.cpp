#include "game.h"
#include "state.h"
#include "utils.h"


GameError init_game(Game* game,
                    const char* assets_dir,
                    const char* win_title,
                    int win_width,
                    int win_height) {

    if (!std::filesystem::exists(assets_dir)) {
        printf("Failed to find assets directory.\n");
        return GAME_ERROR_ASSETS_DIR_DOES_NOT_EXIST;
    }

    game->assets_dir = std::filesystem::path(assets_dir);

    printf("Assets directory path set to %s.\n",
           game->assets_dir.c_str());


    game->window.create(sf::VideoMode(win_width, win_height), win_title);
    game->win_width  = win_width;
    game->win_height = win_height;

    printf("A %dx%d window with title %s created "
           "successfully.\n",
           game->win_width,
           game->win_height,
           win_title);


    printf("Game initialized successfully.\n");


    return GAME_ERROR_NO_ERROR;
}


void quit_game(Game* game) {
    printf("Exitting game...");
}

GameError load_textures(Game* game) {

    std::vector<std::pair<std::filesystem::path, const char*>> files_tags = {
        { game->assets_dir / "bg-v1.png", "bg" },
        { game->assets_dir / "press.png", "press" }
    };

    for (const auto& pair : files_tags) {
        if (!std::filesystem::exists(pair.first)) {
            printf("Failed to find asset %s in the "
                   "filesystem!",
                   pair.first.c_str());
            return GAME_ERROR_FILE_NOT_FOUND;
        }
    }


    for (int i = 0; i < files_tags.size(); i++) {
        sf::Texture texture;
        texture.loadFromFile(files_tags[i].first);

        game->textures[files_tags[i].second] = texture;
    }

    return GAME_ERROR_NO_ERROR;
}

GameError load_fonts(Game* game) {

    std::vector<std::pair<std::filesystem::path, const char*>> files_tags = {
        { game->assets_dir / "Lobster-Regular.ttf", "lobster" },
    };

    for (const auto& pair : files_tags) {
        if (!std::filesystem::exists(pair.first)) {
            printf("Failed to find asset %s in the "
                   "filesystem!",
                   pair.first.c_str());
            return GAME_ERROR_FILE_NOT_FOUND;
        }
    }


    for (int i = 0; i < files_tags.size(); i++) {
        sf::Font font;
        font.loadFromFile(files_tags[i].first);

        game->fonts[files_tags[i].second] = font;
    }

    return GAME_ERROR_NO_ERROR;
}

GameError load_music(Game* game) {
    if (!game->music.openFromFile(game->assets_dir / "bg.mp3")) {
        return GAME_ERROR_FILE_NOT_FOUND;
    }

    return GAME_ERROR_NO_ERROR;
}
