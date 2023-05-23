#version 450 core

in vec2 texcoords;

out vec4 color;

uniform sampler2D image;
uniform float time;

void main() {
    vec4 t = texture(image, texcoords);

    if (t.a < 0.1) {
        color = t;
    } else {
        // sin(freq * time)
        color = vec4(texture(image, texcoords).rgb, abs(sin(0.001 * time)));
    }
    
}
