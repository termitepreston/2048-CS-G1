#include "./utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <SDL_log.h>

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

GameError create_shader_program(std::filesystem::path vert_shader_path,
                                std::filesystem::path frag_shader_path,
                                GLuint* program) {
    GLuint vs, fs;
    GLint compilation_staus, link_status;
    GLchar compilation_log[512];

    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);

    string vert_source, frag_source;

    if (read_whole_file_text(vert_shader_path.c_str(), vert_source) != 0 ||
        read_whole_file_text(frag_shader_path.c_str(), frag_source) != 0) {
        return GAME_ERROR_FILE_NOT_FOUND;
    }


    SDL_LogDebug(SDL_LOG_PRIORITY_INFO,
                 "Vert shader:\n %s\n",
                 vert_source.c_str());
    SDL_LogDebug(SDL_LOG_PRIORITY_INFO,
                 "Frag shader:\n %s\n",
                 frag_source.c_str());

    const char* vert_source_ptr = vert_source.c_str();
    const char* frag_source_ptr = frag_source.c_str();


    glShaderSource(vs, 1, (const GLchar**)&vert_source_ptr, NULL);

    glCompileShader(vs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &compilation_staus);

    if (compilation_staus != GL_TRUE) {
        glGetShaderInfoLog(vs, 512, NULL, compilation_log);
        SDL_Log("Failed to compile vertex shader: %s\n", compilation_log);
        return GAME_ERROR_VERT_SHADER_COMPILATION_FAILED;
    }


    glShaderSource(fs, 1, (const GLchar**)&frag_source_ptr, NULL);

    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &compilation_staus);

    if (compilation_staus != GL_TRUE) {
        glGetShaderInfoLog(fs, 512, NULL, compilation_log);
        SDL_Log("Failed to compile fragment shader: %s\n", compilation_log);
        return GAME_ERROR_FRAG_SHADER_COMPILATION_FAILED;
    }

    *program = glCreateProgram();

    glAttachShader(*program, vs);
    glAttachShader(*program, fs);
    glLinkProgram(*program);

    glGetProgramiv(*program, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        glGetProgramInfoLog(*program, 512, NULL, compilation_log);
        SDL_Log("Failed to link program: %s\n", compilation_log);
        return GAME_ERROR_SHADER_LINKING_FAILED;
    }

    return GAME_ERROR_NO_ERROR;
}
