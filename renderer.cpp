#include "renderer.h"


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

void use_shader(Renderer* renderer, const char* shader) {
    glUseProgram(renderer->shaders[shader]);
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
    glBindTexture(GL_TEXTURE_2D, game->textures.at(tex));

    glBindTextureUnit(0, game->textures.at(tex));

    glBindVertexArray(renderer->sprite_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
