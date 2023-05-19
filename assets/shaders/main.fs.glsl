#version 450 core

layout (location = 0) out vec4 f_color;

uniform uint u_r_val;

void main() {
    f_color = vec4(sin(float(u_r_val)), 0.4, 0.8, 1.0);
}
