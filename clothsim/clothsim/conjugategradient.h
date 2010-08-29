#ifndef CONJUGATEGRADIENT_H // header guards
#define CONJUGATEGRADIENT_H

#include "stdafx.h"
#include "MathTypes.h"

vector3d ConjugateGradient(Matrix3x3 A, vector3d b, vector3d x_start, int max_iterations, float error);
void TakeStep(void);


#endif