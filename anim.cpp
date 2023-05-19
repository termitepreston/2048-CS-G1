#include "./anim.h"


void spring(float& x, float& v, float xt, float zeta, float omega, float dt) {
    const float f       = 1.0f + 2.0f * dt * zeta * omega;
    const float o_sq    = omega * omega;
    const float dt_o_sq = dt * o_sq;
    const float dt_sq_o_sq = dt * dt_o_sq;
    const float det_inv    = 1.0f / (f + dt_sq_o_sq);
    const float det_x = f * x + dt * v + dt_sq_o_sq * xt;
    const float det_v = v + dt_o_sq * (xt - x);
    x                 = det_x * det_inv;
    v                 = det_v * det_inv;
}
