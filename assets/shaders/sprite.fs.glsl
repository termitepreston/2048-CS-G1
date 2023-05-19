#version 450 core

in vec2 texcoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 sprite_color;

void main() {
    // color = vec4(sprite_color, 1.0f);
    // opengl internally works with normalized color values [0.0, 1.0]
    // A = 1.0 means totally opaque.
    color = texture(image, texcoords);
}
