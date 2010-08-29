#ifndef TRIANGLE_H // header guards
#define TRIANGLE_H

// This is the triangle class.
#include "MathTypes.h"

// TRIANGLES ARE DRAWN A -> B -> C (anti-clockwise)
class triangle
{
public:
	int	ai, bi, ci;					// these are the indexes for the verticies.
	float ax, bx, cx, ay, by, cy;	// these are the positions in texture coordinates.
	vector3d *trinormal;

	triangle() {					// constructor function.
		ai = 0; bi = 0; ci = 0;
		ax = 0.0; bx = 0.0; cx = 0.0; ay = 0.0; by = 0.0; cy = 0.0;
		trinormal = new vector3d();
		trinormal->x=0.0;trinormal->y=0.0;trinormal->z=0.0;
	}
	~triangle() {}					// destructor function
};

#endif