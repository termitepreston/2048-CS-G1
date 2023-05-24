#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "utils.h"

#include <filesystem>
#include <stack>
#include <unordered_map>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


struct Game {
    sf::RenderWindow window;
    int win_width, win_height;
    std::filesystem::path assets_dir;
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
    sf::Music music;
    std::stack<State> states;
};

GameError init_game(Game* game,
                    const char* assets_dir,
                    const char* win_title,
                    int win_width,
                    int win_height);

GameError load_textures(Game* game);
GameError load_fonts(Game* game);
GameError load_music(Game* game);

void quit_game(Game* game);


#endif // !GAME_H
