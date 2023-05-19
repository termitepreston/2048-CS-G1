#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <vector>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

enum GameError {
    GAME_ERROR_NO_ERROR = 0,
    GAME_ERROR_ASSETS_DIR_DOES_NOT_EXIST,
    GAME_ERROR_SDL_INIT_FAILED,
    GAME_ERROR_WINDOW_CREATION_FAILED,
    GAME_ERROR_OPENGL_CONTEXT_CREATION_FAILED,
    GAME_ERROR_FAILED_TO_OPEN_AUDIO_DEVICE,
    GAME_ERROR_FILE_NOT_FOUND,
    GAME_ERROR_VERT_SHADER_COMPILATION_FAILED,
    GAME_ERROR_FRAG_SHADER_COMPILATION_FAILED,
    GAME_ERROR_SHADER_LINKING_FAILED
};

/*
 * Opens a specified file and reads the whole thing into a a string.
 *
 * returns 0 on success.
 */

GameError read_whole_file_text(const char* file_path, std::string& out);


GameError read_whole_file_binary(const char* file_path,
                                 std::vector<char>& out);


/*
 * Opens shader source from shader assets directory and
 * builds opengl shader program.
 */
GameError create_shader_program(std::filesystem::path vert_shader_path,
                                std::filesystem::path frag_shader_path,
                                GLuint* program);

#endif // !UTILS_H
