#include "./math.h"
#include <SDL_log.h>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>

using namespace std;


void mat4x4_ortho(Mat4x4 out,
                  float left,
                  float right,
                  float bottom,
                  float top,
                  float znear,
                  float zfar) {
    out[0][0] = 2.0f / (right - left);
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = -(right + left) / (right - left);


    out[1][1] = 2.0f / (top - bottom);
    out[1][0] = 0.0f;
    out[1][2] = 0.0f;
    out[1][3] = -(top + bottom) / (top - bottom);

    out[2][2] = -2.0f / (zfar - znear);
    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][3] = -(zfar + znear) / (zfar - znear);

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}


inline void swap(Mat4x4 m, int x, int y, int w, int z) {
    float temp = m[x][y];
    m[x][y]    = m[w][z];
    m[w][z]    = temp;
}

void mat4x4_transpose(Mat4x4 out) {
    swap(out, 0, 1, 1, 0);
    swap(out, 0, 2, 2, 0);
    swap(out, 0, 3, 3, 0);

    swap(out, 1, 2, 2, 1);
    swap(out, 1, 3, 3, 1);

    swap(out, 2, 3, 3, 2);
}


void mat4x4_trans(Mat4x4 out, float x, float y, float z) {
    out[0][0] = 1.0f;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = x;

    out[1][0] = 0.0f;
    out[1][1] = 1.0f;
    out[1][2] = 0.0f;
    out[1][3] = y;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = 1.0f;
    out[2][3] = z;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

void mat4x4_rotate_z(Mat4x4 out, float angle) {

    out[0][0] = cosf(angle);
    out[0][1] = -sinf(angle);
    out[0][2] = 0.0f;
    out[0][3] = 0.0f;

    out[1][0] = sinf(angle);
    out[1][1] = cosf(angle);
    out[1][2] = 0.0f;
    out[1][3] = 0.0f;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = 1.0f;
    out[2][3] = 0.0f;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

void mat4x4_scale(Mat4x4 out, float x, float y, float z) {

    out[0][0] = x;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = 0.0f;

    out[1][0] = 0.0f;
    out[1][1] = y;
    out[1][2] = 0.0f;
    out[1][3] = 0.0f;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = z;
    out[2][3] = 0.0f;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

void matmul(Mat4x4 l, Mat4x4 r) {
    Mat4x4 t;
    mempcpy(t[0], l[0], sizeof(float) * 16);

    l[0][0] = (t[0][0] * r[0][0]) + (t[0][1] * r[1][0]) +
        (t[0][2] * r[2][0]) + (t[0][3] * r[3][0]);

    l[0][1] = (t[0][0] * r[0][1]) + (t[0][1] * r[1][1]) +
        (t[0][2] * r[2][1]) + (t[0][3] * r[3][1]);

    l[0][2] = (t[0][0] * r[0][2]) + (t[0][1] * r[1][2]) +
        (t[0][2] * r[2][2]) + (t[0][3] * r[3][2]);

    l[0][3] = (t[0][0] * r[0][3]) + (t[0][1] * r[1][3]) +
        (t[0][2] * r[2][3]) + (t[0][3] * r[3][3]);


    l[1][0] = (t[1][0] * r[0][0]) + (t[1][1] * r[1][0]) +
        (t[1][2] * r[2][0]) + (t[1][3] * r[3][0]);

    l[1][1] = (t[1][0] * r[0][1]) + (t[1][1] * r[1][1]) +
        (t[1][2] * r[2][1]) + (t[1][3] * r[3][1]);

    l[1][2] = (t[1][0] * r[0][2]) + (t[1][1] * r[1][2]) +
        (t[1][2] * r[2][2]) + (t[1][3] * r[3][2]);

    l[1][3] = (t[1][0] * r[0][3]) + (t[1][1] * r[1][3]) +
        (t[1][2] * r[2][3]) + (t[1][3] * r[3][3]);

    l[2][0] = (t[2][0] * r[0][0]) + (t[2][1] * r[1][0]) +
        (t[2][2] * r[2][0]) + (t[2][3] * r[3][0]);

    l[2][1] = (t[2][0] * r[0][1]) + (t[2][1] * r[1][1]) +
        (t[2][2] * r[2][1]) + (t[2][3] * r[3][1]);

    l[2][2] = (t[2][0] * r[0][2]) + (t[2][1] * r[1][2]) +
        (t[2][2] * r[2][2]) + (t[2][3] * r[3][2]);

    l[2][3] = (t[2][0] * r[0][3]) + (t[2][1] * r[1][3]) +
        (t[2][2] * r[2][3]) + (t[2][3] * r[3][3]);

    l[3][0] = (t[3][0] * r[0][0]) + (t[3][1] * r[1][0]) +
        (t[3][2] * r[2][0]) + (t[3][3] * r[3][0]);

    l[3][1] = (t[3][0] * r[0][1]) + (t[3][1] * r[1][1]) +
        (t[3][2] * r[2][1]) + (t[3][3] * r[3][1]);

    l[3][2] = (t[3][0] * r[0][2]) + (t[3][1] * r[1][2]) +
        (t[3][2] * r[2][2]) + (t[3][3] * r[3][2]);

    l[3][3] = (t[3][0] * r[0][3]) + (t[3][1] * r[1][3]) +
        (t[3][2] * r[2][3]) + (t[3][3] * r[3][3]);
}

void identity(Mat4x4 out) {
    mat4x4_identity(out);
}

void translate(Mat4x4 in, Vec2 position) {
    // out = in * tansl_mat
    Mat4x4 temp;

    mat4x4_trans(temp, position.x, position.y, 0.0f);

    matmul(in, temp);
}

void scale(Mat4x4 in, Vec2 size) {
    Mat4x4 temp;

    mat4x4_scale(temp, size.x, size.y, 1.0f);

    matmul(in, temp);
}

void rotatez(Mat4x4 in, float angle) {
    Mat4x4 temp;

    mat4x4_rotate_z(temp, angle);

    matmul(in, temp);
}

void mat4x4_mul(Mat4x4 out, Mat4x4 left, Mat4x4 right) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            out[r][c] = 0;
            for (int i = 0; i < 4; i++) {
                out[r][c] += left[r][i] * right[i][c];
            }
        }
    }
}

void mat4x4_identity(Mat4x4 out) {

    out[0][0] = 1.0f;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = 0.0f;

    out[1][0] = 0.0f;
    out[1][1] = 1.0f;
    out[1][2] = 0.0f;
    out[1][3] = 0.0f;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = 1.0f;
    out[2][3] = 0.0f;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

void print_mat4x4(const char* typ, Mat4x4 mat) {
    stringstream sout;

    sout << typ << ": "
         << "[\n\t";

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 3; c++) {
            sout << setw(7) << right << fixed
                 << setprecision(4) << mat[r][c] << ",";
        }
        sout << mat[r][3] << "\n\t";
    }
    sout << "]";

    SDL_Log("%s\n", sout.str().c_str());
}
