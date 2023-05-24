#include "./utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>


using namespace std;

GameError read_whole_file_text(const char* file_path, std::string& out) {
    ifstream fin;

    fin.open(file_path);

    if (!fin.is_open()) return GAME_ERROR_FILE_NOT_FOUND;


    stringstream sin;
    sin << fin.rdbuf();

    out = sin.str();

    fin.close();

    return GAME_ERROR_NO_ERROR;
}

GameError read_whole_file_binary(const char* file_path,
                                 std::vector<char>& out) {
    ifstream fin;

    fin.open(file_path, ios::binary);

    if (!fin.is_open()) return GAME_ERROR_FILE_NOT_FOUND;

    fin.seekg(0, ios::end);

    unsigned int file_size = fin.tellg();
    out.reserve(file_size);

    fin.seekg(0, ios::beg);

    fin.read(out.data(), file_size);

    return GAME_ERROR_NO_ERROR;
}
