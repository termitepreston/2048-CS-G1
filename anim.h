#ifndef ANIM_H
#define ANIM_H

/*
 * x - value (input/output)
 * v - velocity (input/output)
 * xt - target value (input)
 * zeta - damping ratio (input)
 * omega - angular frequency (input)
 * dt - timestep (input)
 */
void spring(float& x, float& v, float xt, float zeta, float omega, float dt);

#endif
