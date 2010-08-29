/////////////////////////////////////////////////////////////////////////////
//
//	matrix4.h
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	4x4 matrix class
//
//	revision history:
//	Sept 29, 2005		Code by Hamilton Chong
//	Oct   1, 2005		Update to have z-rotation and more * overloading
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MATRIX4_H
#define MATRIX4_H

#include <stdio.h>
#include <math.h>
#include <glut.h>
#include "coords3.h"

//////////////////////////////////////////////////
// matrix4										//
// -------------------------------------------- //
// class for dealing with 4x4 matrices.			//
// conviniently acts on coords3 types.			//
//////////////////////////////////////////////////

class matrix4
{
public:
	double elt[4][4];

public:
	void SetIdentity(void)	{ elt[0][0]=1.0; elt[0][1]=0.0; elt[0][2]=0.0; elt[0][3]=0.0;
							  elt[1][0]=0.0; elt[1][1]=1.0; elt[1][2]=0.0; elt[1][3]=0.0;
							  elt[2][0]=0.0; elt[2][1]=0.0; elt[2][2]=1.0; elt[2][3]=0.0;
							  elt[3][0]=0.0; elt[3][1]=0.0; elt[3][2]=0.0; elt[3][3]=1.0; }

	matrix4() { SetIdentity(); }
	matrix4(const matrix4& in) 
	{
		elt[0][0]=in.elt[0][0]; 
		elt[0][1]=in.elt[0][1]; 
		elt[0][2]=in.elt[0][2]; 
		elt[0][3]=in.elt[0][3];

		elt[1][0]=in.elt[1][0]; 
		elt[1][1]=in.elt[1][1]; 
		elt[1][2]=in.elt[1][2]; 
		elt[1][3]=in.elt[1][3];

		elt[2][0]=in.elt[2][0]; 
		elt[2][1]=in.elt[2][1]; 
		elt[2][2]=in.elt[2][2]; 
		elt[2][3]=in.elt[2][3];

		elt[3][0]=in.elt[3][0]; 
		elt[3][1]=in.elt[3][1]; 
		elt[3][2]=in.elt[3][2]; 
		elt[3][3]=in.elt[3][3];
	}
	matrix4(double m00, double m01, double m02, double m03, 
			  double m10, double m11, double m12, double m13, 
			  double m20, double m21, double m22, double m23, 
			  double m30, double m31, double m32, double m33 ) 
	{
		elt[0][0] = m00; 
		elt[0][1] = m01; 
		elt[0][2] = m02; 
		elt[0][3] = m03;

		elt[1][0] = m10; 
		elt[1][1] = m11; 
		elt[1][2] = m12; 
		elt[1][3] = m13;

		elt[2][0] = m20; 
		elt[2][1] = m21; 
		elt[2][2] = m22; 
		elt[2][3] = m23;

		elt[3][0] = m30; 
		elt[3][1] = m31; 
		elt[3][2] = m32; 
		elt[3][3] = m33;
	}
	matrix4(GLdouble *m) 
	{
		// assume m is non-null and really 
		// has 16 double values
		elt[0][0] = m[0]; 
		elt[0][1] = m[4]; 
		elt[0][2] = m[8]; 
		elt[0][3] = m[12];

		elt[1][0] = m[1]; 
		elt[1][1] = m[5]; 
		elt[1][2] = m[9]; 
		elt[1][3] = m[13];

		elt[2][0] = m[2]; 
		elt[2][1] = m[6]; 
		elt[2][2] = m[10]; 
		elt[2][3] = m[14];

		elt[3][0] = m[3]; 
		elt[3][1] = m[7]; 
		elt[3][2] = m[11]; 
		elt[3][3] = m[15];
	}
	matrix4(GLfloat *m) 
	{
		// assume m is non-null and really 
		// has 16 double values
		elt[0][0] = m[0]; 
		elt[0][1] = m[4]; 
		elt[0][2] = m[8]; 
		elt[0][3] = m[12];

		elt[1][0] = m[1]; 
		elt[1][1] = m[5]; 
		elt[1][2] = m[9]; 
		elt[1][3] = m[13];

		elt[2][0] = m[2]; 
		elt[2][1] = m[6]; 
		elt[2][2] = m[10]; 
		elt[2][3] = m[14];

		elt[3][0] = m[3]; 
		elt[3][1] = m[7]; 
		elt[3][2] = m[11]; 
		elt[3][3] = m[15];
	}
	~matrix4() {}

public:
	// public member functions

	// THESE NEXT THREE ARE ADDED FUNCTIONS!!!!
	coords3		GetRotation(bool b_tagup, bool b_tagdown, bool c_tagup, bool c_tagdown, bool d_tagup, bool d_tagdown)	const;
	coords3		GetTranslation(void)									const;
	void		PrintMatrix(void)										const;

	coords3		Mult(const coords3 &in)					const;
	matrix4		Mult(const matrix4 &in)					const;
	matrix4		GetAffineInverse(bool *bError=(bool*)0)	const;
	matrix4		GetTranspose(void)						const;
	matrix4		GetNormalMatrix(void)					const;
	void		Mat2GL(GLdouble *array)					const;
	void		Mat2GL(GLfloat *array)					const;

	// operators
	matrix4& operator += (const matrix4 &rhs);
	matrix4& operator -= (const matrix4 &rhs);
	matrix4& operator *= (double scale);
};

inline const matrix4 operator +(const matrix4 &lhs, const matrix4 &rhs)
{
	return matrix4(lhs) += rhs;
}

inline const matrix4 operator -(const matrix4 &lhs, const matrix4 &rhs)
{
	return matrix4(lhs) -= rhs;
}

inline const matrix4 operator *(const matrix4 &lhs, double rhs)
{
	return matrix4(lhs) *= rhs;
}

inline const matrix4 operator *(double lhs, const matrix4 &rhs)
{
	return matrix4(rhs) *= lhs;
}

inline const matrix4 operator *(const matrix4 &lhs, const matrix4 &rhs)
{
	return lhs.Mult(rhs);
}

inline const coords3 operator *(const matrix4 &lhs, const coords3 &rhs)
{
	return lhs.Mult(rhs);
}


// P R O T O T Y P E S /////////////////////////////////////////////////

// THESE NEXT FUNCTIONS ARE TO HELP US FIND ONLY THE ROTATION OR TRANSLATION MATRICIES 
// IN A 4X4 AFINE MATRIX
matrix4		GetRotation(const matrix4 input);
matrix4		GetTranslation(const matrix4 input);

matrix4		MakeXRotation(double deg);
matrix4		MakeYRotation(double deg);
matrix4		MakeZRotation(double deg);
matrix4		MakeTranslation(double tx, double ty, double tz);
matrix4		MakeScale(double sx, double sy, double sz);
matrix4		MakeProjection (double top, double bot, 
							double left, double right, 
							double nval, double fval);
matrix4		MakeProjection (double fovy, double ar,
							double zNear, double zFar);


#endif 

