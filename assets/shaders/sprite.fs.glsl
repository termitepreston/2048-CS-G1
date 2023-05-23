#version 450 core

in vec2 texcoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 sprite_color;
uniform float time;

float plot(vec2 st, float pct) {
    float param = 0.01;
    return smoothstep(pct-param, pct, st.y) - smoothstep(pct, pct+param, st.y);
}

// p: point.
// hs: half size
// r: corner radius
float box(vec2 p, vec2 hs, float r) {
    vec2 q = abs(p) - hs + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

// p: point.
// r: half_size of r
float box_out(vec2 r, vec2 p) {
    vec2 q = abs(p) - r;
    return length(max(q, 0));
}

void main_image(out vec4 frag_color, in vec2 frag_coord) {
    vec2 res = vec2(480, 640);
    vec2 pos = (2.0 * frag_coord - res.xy) / res.y;

    vec2 half_size = vec2(0.7, 0.4);
    float corner_radius = ((cos(time * 0.005) + 1.0) * 0.5) * 0.4;
    float d = box(pos, half_size, corner_radius);


    vec3 color = vec3(1.0) - sign(d) * vec3(0.1, 0.4, 0.7);
    color *= 1.0 - exp(-4.0 * abs(d));
    color *= 0.8 * 0.2 * cos(180.0 * d);
    color = mix(color, vec3(1.0), 1.0 - smoothstep(0.0, 3.0, (abs(d) * 0.003) * res.y));

    frag_color = vec4(color, 1.0);
}

void circle(out vec4 frag_color, in vec2 frag_coord, vec2 res) {
    float pct = box(frag_coord / vec2(480,640), vec2(0.5,0.5), 0.1); 

    frag_color = vec4(vec3(pct), 1.0);
}

float boxn(in vec2 _st, in vec2 _size) {
    _size = vec2(0.5) - _size*0.5;

    vec2 uv = smoothstep(_size, _size+vec2(0.001), _st);
    uv *= smoothstep(_size, _size+vec2(0.001), vec2(1.0)-_st);

    return uv.x * uv.y;
}

flaot cross(in vec2 _st, float _size) {
    return boxn(_st, vec2(_size,_size/4.)) + boxn(_st, vec2(_size/4.,_size));
    
}

void box_img(out vec4 frag_color, in vec2 frag_coord) {
    vec2 st = frag_coord / vec2(480.0,640.0);
    vec3 color = vec3(0.0);

}

void image0(out vec4 frag_color, in vec2 frag_coord) {
// color = vec4(sprite_color, 1.0f);
    // opengl internally works with normalized color values [0.0, 1.0]
    // A = 1.0 means totally opaque.
    // color = texture(image, texcoords);
    vec2 st = frag_coord.xy / vec2(480, 640);

    // mapping x's value from the normalized coordinate to a color.
    //float y = pow(st.x,6.0);
    //float y = step(0.5,st.x);
    //float y = smoothstep(0.1, 0.9, st.x);
    float y = sin(st.x * 3.5);


    vec3 pcolor = vec3(y);

    float pct = plot(st,y);

    // linearly interpolate between some shade of gray and green.
    pcolor = (1.0-pct)*pcolor+pct*vec3(0.0, 1.0, 0.0);

    frag_color = vec4(pcolor,1.0);
}

void main() {
   // image0(color, gl_FragCoord.xy); 
   // main_image(color, gl_FragCoord.xy);
    circle(color, gl_FragCoord.xy, vec2(480.0, 640.0));
}
