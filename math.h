#ifndef MATH_H
#define MATH_H

typedef float Mat4x4[4][4];

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
};

void print_mat4x4(const char* typ, Mat4x4 mat);

void mat4x4_ortho(Mat4x4 out,
                  float left,
                  float right,
                  float bottom,
                  float top,
                  float znear,
                  float zfar);

void mat4x4_trans(Mat4x4 out, float x, float y, float z);

void mat4x4_transpose(Mat4x4 out);

void mat4x4_identity(Mat4x4 out);

void mat4x4_mul(Mat4x4 out, Mat4x4 left, Mat4x4 right);

void matmul(Mat4x4 left, Mat4x4 right);

void identity(Mat4x4 out);

void translate(Mat4x4 in, Vec2 position);

void scale(Mat4x4 in, Vec2 size);

void rotatez(Mat4x4 in, float angle);

#endif // !MATH_H
