/////////////////////////////////////////////////////////////////////////////
//
//	qrot.h
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	Quaternion rotation class
//
//	revision history:
//	Oct 26, 2005		Code by Hamilton Chong
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef	QROT_H
#define QROT_H

#include <glut.h>
#include "coords3.h"
#include "matrix4.h"


//////////////////////////////////////////////////////////
// qrot													//
// ---------------------------------------------------- //
// class for handling 3D quaternion rotations			//
//														//
//////////////////////////////////////////////////////////

class qrot
{
public:
	double	x,y,z,w;
public:
	// constructors and destructor
	qrot():x(0.0), y(0.0), z(0.0), w(1.0) {}
	qrot(double fx, double fy, double fz, double fw):x(fx),y(fy),z(fz),w(fw) {}
	qrot(const qrot& in) : x(in.x), y(in.y), z(in.z), w(in.w) {}
	~qrot() {}

	// public interface prototypes
	void		Normalize(void);
	void		Qrot2GL(GLdouble *array)	const;
	void		Qrot2GL(GLfloat *array)		const;
	coords3		Apply(const coords3& in)	const;
	qrot		GetInverse(void)			const;
	qrot		Compose(const qrot& in)		const;
	matrix4		Q2mat(void)					const;
	qrot		RaisePower(double alpha)	const;

	// operators
	qrot& operator += (const qrot &rhs);
	qrot& operator -= (const qrot &rhs);
	qrot& operator *= (double scale);

	// friendly functions
	friend double dot(const qrot& q1, const qrot& q2)
	{ return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w; }
};

inline const qrot operator +(const qrot &lhs, const qrot &rhs)
{
	return qrot(lhs) += rhs;
}

inline const qrot operator -(const qrot &lhs, const qrot &rhs)
{
	return qrot(lhs) -= rhs;
}

inline const qrot operator *(const qrot &lhs, double rhs)
{
	return qrot(lhs) *= rhs;
}

inline const qrot operator *(double lhs, const qrot &rhs)
{
	return qrot(rhs) *= lhs;
}

inline const qrot operator *(const qrot &lhs, const qrot &rhs)
{
	return lhs.Compose(rhs);
}

inline const coords3 operator *(const qrot &lhs, const coords3 &rhs)
{
	return lhs.Apply(rhs);
}

///////////////////////////////////////////////////////////////////////

qrot QrotIdent(void);
qrot MakeXQrot(double deg);
qrot MakeYQrot(double deg);
qrot MakeZQrot(double deg);

#endif
