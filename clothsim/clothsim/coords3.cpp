/////////////////////////////////////////////////////////////////////////////
//
//	coords3.cpp
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	3D coordinate class.
//
//	revision history:
//	Sept 29, 2005		Code by Hamilton Chong
//
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <math.h>
#include <assert.h>
#include "coords3.h"

//////////////////////////////////////////
//										//
//	coords3								//
//										//
//////////////////////////////////////////


coords3& coords3::operator += (const coords3 &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

coords3& coords3::operator -= (const coords3 &rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

coords3& coords3::operator *= (double scale)
{
	x *= scale;
	y *= scale;
	z *= scale;

	return *this;
}

coords3 CrossProduct(const coords3& p, const coords3& q)
{
	// cross_product is defined on vectors in 3D
	return coords3(p.y * q.z - p.z * q.y, 
				   p.z * q.x - p.x * q.z, 
				   p.x * q.y - p.y * q.x);
}
