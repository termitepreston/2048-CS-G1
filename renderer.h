#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"
#include "math.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

#include <unordered_map>

struct Renderer {
    std::unordered_map<std::string, GLuint> shaders;
    GLuint sprite_vao;
    GLuint sprite_vbo;
};

void use_shader(Renderer* renderer, const char* shader);

void add_shader(Renderer* renderer,
                std::pair<const char*, GLuint> program);

void init_renderer(Renderer* renderer);

void render_sprite(Game* game,
                   const char* tex,
                   Renderer* renderer,
                   const char* shader,
                   Vec2 position,
                   Vec2 size,
                   float rotate,
                   Vec3 color);
#endif
