/////////////////////////////////////////////////////////////////////////////
//
//	qrot.cpp
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	quaternion class
//
//	revision history:
//	Oct 26, 2005		Code by Hamilton Chong
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "qrot.h"
#include <assert.h>
#include "matrix4.h"

//////////////////////////////////////////
//										//
//	qrot								//
//										//
//////////////////////////////////////////

void qrot::Normalize(void)
{
	double mag = (double)sqrt(x*x + y*y + z*z + w*w);
	if(mag == 0.0) return;
	double mag_inv = (double)1.0 / mag;
	x *= mag_inv;
	y *= mag_inv;
	z *= mag_inv;
	w *= mag_inv;
}

qrot& qrot::operator += (const qrot &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;

	return *this;
}

qrot& qrot::operator -= (const qrot &rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;

	return *this;
}

qrot& qrot::operator *= (double scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;

	return *this;
}

void	qrot::Qrot2GL(GLdouble *array)		const
{
	// currently, we lazily create the matrix and use matrix routine
	// for converting to opengl array
	matrix4 mat_rep = Q2mat();
	mat_rep.Mat2GL(array);
}

void	qrot::Qrot2GL(GLfloat *array)		const
{
	// currently, we lazily create the matrix and use matrix routine
	// for converting to opengl array
	matrix4 mat_rep = Q2mat();
	mat_rep.Mat2GL(array);
}

coords3	qrot::Apply(const coords3& in)		const
{
	qrot vec(in.x, in.y, in.z, 0.0);
	qrot quat_rep = Compose(vec.Compose(GetInverse()));
	return coords3(quat_rep.x, quat_rep.y, quat_rep.z);
}

qrot	qrot::GetInverse(void)				const
{
	double dotprod = dot(*this, *this);
	assert(dotprod != 0.0);
	if(dotprod == 0.0) {
		// error!
		return qrot(0.0, 0.0, 0.0, 0.0);
	}
	double dot_inv = 1.0 / dotprod;
	return qrot(-dot_inv * x, -dot_inv * y, -dot_inv * z, dot_inv * w);
}

qrot	qrot::Compose(const qrot& in)		const
{
	coords3 u(x, y, z);
	coords3 v(in.x, in.y, in.z);
	double new_w = w * in.w - DotProduct(u, v);
	coords3 new_vector = ((w * v) + (in.w * u)) + CrossProduct(u, v);
	return qrot(new_vector.x, new_vector.y, new_vector.z, new_w);
}

matrix4	qrot::Q2mat(void)					const
{
	matrix4 ret;

	double dotprod = dot(*this, *this);
	if(dotprod == 0.0)
		return matrix4(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	double s = 2.0 / dotprod;
	double xs = x * s;
	double ys = y * s;
	double zs = z * s;
	
	double wx = w * xs;
	double wy = w * ys;
	double wz = w * zs;
	
	double xx = x * xs;
	double xy = x * ys;
	double xz = x * zs;
	
	double yy = y * ys;
	double yz = y * zs;
	double zz = z * zs;
	
	// set up the matrix
#define XIND 0
#define YIND 1
#define ZIND 2
#define WIND 3
	ret.elt[XIND][XIND] = 1.0 - (yy + zz);
	ret.elt[XIND][YIND] = xy - wz;
	ret.elt[XIND][ZIND] = xz + wy;
	ret.elt[YIND][XIND] = xy + wz;
	ret.elt[YIND][YIND] = 1.0 - (xx + zz);
	ret.elt[YIND][ZIND] = yz - wx;
	ret.elt[ZIND][XIND] = xz - wy;
	ret.elt[ZIND][YIND] = yz + wx;
	ret.elt[ZIND][ZIND] = 1.0 - (xx + yy);
	
	// no translation; and affine matrix
	ret.elt[XIND][WIND] = 0.0;
	ret.elt[YIND][WIND] = 0.0;
	ret.elt[ZIND][WIND] = 0.0;
	ret.elt[WIND][XIND] = ret.elt[WIND][YIND] = ret.elt[WIND][ZIND] = 0.0;
	ret.elt[WIND][WIND] = 1.0;
#undef XIND
#undef YIND
#undef ZIND
#undef WIND

	return ret;
}

qrot QrotIdent(void) { return qrot(0.0,0.0,0.0,1.0); }

qrot MakeXQrot(double deg)
{
	qrot ret;
	double half_rad = deg * PI / 360.0;
	ret.x = sin(half_rad);
	ret.y = 0.0;
	ret.z = 0.0;
	ret.w = cos(half_rad);
	return ret;
}

qrot MakeYQrot(double deg)
{
	qrot ret;
	double half_rad = deg * PI / 360.0;
	ret.x = 0.0;
	ret.y = sin(half_rad);
	ret.z = 0.0;
	ret.w = cos(half_rad);
	return ret;
}

qrot MakeZQrot(double deg)
{
	qrot ret;
	double half_rad = deg * PI / 360.0;
	ret.x = 0.0;
	ret.y = 0.0;
	ret.z = sin(half_rad);
	ret.w = cos(half_rad);
	return ret;
}

// this does the qrot^a calculation
qrot qrot::RaisePower(double alpha)			const
{
	qrot ret;
	// get the theta value
	double theta = 2 * acos (w);
	ret.w = cos (alpha * theta / 2);
	// find the vector x, y & z values without the sin(alpha * theta / 2) term
	double divisor = sin (theta / 2);
	if (divisor == 0) {
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
	} else {
		ret.x = x / divisor;
		ret.y = y / divisor;
		ret.z = z / divisor;
		// now find the actual return values
		double multiplier =  sin (alpha * theta / 2);
		ret.x = ret.x * multiplier;
		ret.y = ret.y * multiplier;
		ret.z = ret.z * multiplier;
	}
	return (ret);
}