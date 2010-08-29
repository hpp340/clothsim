/////////////////////////////////////////////////////////////////////////////
//
//	matrix4.cpp
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
#include "stdafx.h"
#include "matrix4.h"

//////////////////////////////////////////
//										//
//	matrix4								//
//										//
//////////////////////////////////////////

coords3	matrix4::Mult(const coords3 &in) const
{
	return coords3( elt[0][0] * in.x + elt[0][1] * in.y + elt[0][2] * in.z + elt[0][3] * 1.0,
					elt[1][0] * in.x + elt[1][1] * in.y + elt[1][2] * in.z + elt[1][3] * 1.0, 
					elt[2][0] * in.x + elt[2][1] * in.y + elt[2][2] * in.z + elt[2][3] * 1.0);
}

matrix4	matrix4::Mult(const matrix4 &in) const
{
	return matrix4(elt[0][0] * in.elt[0][0] + elt[0][1] * in.elt[1][0] + elt[0][2] * in.elt[2][0] + elt[0][3] * in.elt[3][0],
					 elt[0][0] * in.elt[0][1] + elt[0][1] * in.elt[1][1] + elt[0][2] * in.elt[2][1] + elt[0][3] * in.elt[3][1],
					 elt[0][0] * in.elt[0][2] + elt[0][1] * in.elt[1][2] + elt[0][2] * in.elt[2][2] + elt[0][3] * in.elt[3][2],
					 elt[0][0] * in.elt[0][3] + elt[0][1] * in.elt[1][3] + elt[0][2] * in.elt[2][3] + elt[0][3] * in.elt[3][3],
					 
					 elt[1][0] * in.elt[0][0] + elt[1][1] * in.elt[1][0] + elt[1][2] * in.elt[2][0] + elt[1][3] * in.elt[3][0],
					 elt[1][0] * in.elt[0][1] + elt[1][1] * in.elt[1][1] + elt[1][2] * in.elt[2][1] + elt[1][3] * in.elt[3][1],
					 elt[1][0] * in.elt[0][2] + elt[1][1] * in.elt[1][2] + elt[1][2] * in.elt[2][2] + elt[1][3] * in.elt[3][2],
					 elt[1][0] * in.elt[0][3] + elt[1][1] * in.elt[1][3] + elt[1][2] * in.elt[2][3] + elt[1][3] * in.elt[3][3],
					 
					 elt[2][0] * in.elt[0][0] + elt[2][1] * in.elt[1][0] + elt[2][2] * in.elt[2][0] + elt[2][3] * in.elt[3][0],
					 elt[2][0] * in.elt[0][1] + elt[2][1] * in.elt[1][1] + elt[2][2] * in.elt[2][1] + elt[2][3] * in.elt[3][1],
					 elt[2][0] * in.elt[0][2] + elt[2][1] * in.elt[1][2] + elt[2][2] * in.elt[2][2] + elt[2][3] * in.elt[3][2],
					 elt[2][0] * in.elt[0][3] + elt[2][1] * in.elt[1][3] + elt[2][2] * in.elt[2][3] + elt[2][3] * in.elt[3][3],
					 
					 elt[3][0] * in.elt[0][0] + elt[3][1] * in.elt[1][0] + elt[3][2] * in.elt[2][0] + elt[3][3] * in.elt[3][0],
					 elt[3][0] * in.elt[0][1] + elt[3][1] * in.elt[1][1] + elt[3][2] * in.elt[2][1] + elt[3][3] * in.elt[3][1],
					 elt[3][0] * in.elt[0][2] + elt[3][1] * in.elt[1][2] + elt[3][2] * in.elt[2][2] + elt[3][3] * in.elt[3][2],
					 elt[3][0] * in.elt[0][3] + elt[3][1] * in.elt[1][3] + elt[3][2] * in.elt[2][3] + elt[3][3] * in.elt[3][3] );
}


/* get the rotational value.  This is going to be done by throwing away the RHS of the 4x4 matrix and computing
   the arsin and arcos values.       ADDED BY JONATHAN TOMPSON,  Tested and passed: 10/23/'05

   OK, we must recognise that for any rotation represented with euler angles we have A = BCD where:
	-  B is a 'b' degrees rotation abou the x-axis.
	-  C is a 'c' degrees rotation abou the y-axis.
	-  D is a 'd' degrees rotation abou the z-axis.
	-  B =		1		0		0
				0		cosb	sinb
				0		-sinb	cosb
	-  C =		cosc	0		-sinc
				0		1		0
				sinc	0		cosc
	-  D =		cosd	sind	0
				-sind	cosd	0
				0		0		1
	Therefore (by hand... I hope this is right) A =		(cosccosd)				(coscsind)				-sinc
														(sinbsinccosd-cosbsind)	(sinbsincsind+cosbcosd)	(sinbcosc)
														(cosbsinccosd+sinbsind)	(cosbsincsind-sinbcosd)	(cosbcosc)
	From this there are a number of ways of finding the euler angles, here is one way:
	c = - arsine( A[0][2] );
	b = arcos( A[2][2] / cos(c) )	OR asin( A[1][2] / cos(c));
	d = arsine( A[0][1] / cos(c) );

// NOTE: AFTER ALL THAT, THIS FUNCTION WASN'T EVEN USED.  IT WAS HIGHLY LIMITED BY THE OUTPUT RANGE OF THE
//		 ASIN AND ACOS FUNCTIONS...  YOU CANNOT DESCRIBE 360 DEGREE ROTATIONS USING THIS.  ONLY FROM -90 -> 90.
//		 THERE WAS A WORK AROUND, BY HAVING THE TAGS, BUT THAT GOT MESSY SO I WENT BACK TO MATRIX REPRESENTATION
//		 FOR THE ROBOTS BODY AND THE SKY CAMERA.
*/

coords3	matrix4::GetRotation(bool b_tagup, bool b_tagdown, bool c_tagup, bool c_tagdown, bool d_tagup, bool d_tagdown) const{
	// NOTE: Since acos and asin cannot go passed 180 or 0 degress we send these long lines of tags to
	//		 tell GetRotation where to go when we reach these end values


	coords3 ret;	// this will be the return value
	long double b_val, c_val, d_val;	// the euler angles

	// check that the matrix is indeed affine
	if( (elt[3][0] != 0.0) || (elt[3][1] != 0.0) || (elt[3][2] != 0.0) || (elt[3][3] != 1.0) )
	{
		printf("GetRotation was given something other than an affine matrix!!!\n");
		// return a rotation of zero,  default value!
		ret.SetCoords(0.0, 0.0, 0.0);
		return ret;
	}

	// find the values for ret and return them
	c_val = (- asin(elt[0][2]))* 180.0/PI;

	if(cos(c_val * PI/180.0) != 0.0) { // check we're not dividing by zero
		// THESE NEXT TWO IF STATEMENTS GET RID OF A KNOWN BUG
		// -> if the value to go to acos get close to 1 or -1
		// then it returns a "1.#IND00".  I looked everywhere on the net to find out what this means, but
		// I couldn't find it.  I'm guessing its some sort of float stack overflow OR something wrong with type
		// casting from long double to a double.
		if((elt[1][2]) / (cos(c_val * PI/180.0)) > 0.9999999) {  // acos(1) = 90
			b_val = 90.0;
		}else if(((elt[1][2]) / (cos(c_val * PI/180.0))) < -0.9999999) {  // acos(-1) = -90
			b_val = -90.0;
		} else {
			b_val = (asin(((elt[1][2]) / (cos(c_val * PI/180.0)))) * 180.0/PI);}
	} else {
		printf("ERROR: division by zero in GetRotation\n");
	}

	if(cos(c_val * PI/180.0) != 0.0) { // check we're not dividing by zero
		if((elt[0][1]) / (cos(c_val * PI/180.0)) > 0.9999999) {  // acos(1) = 90
			d_val = 90.0;
		}else if(((elt[0][1]) / (cos(c_val * PI/180.0))) < -0.9999999) {  // acos(-1) = -90
			d_val = -90.0;
		} else {
			d_val = (asin(((elt[0][1]) / (cos(c_val * PI/180.0)))) * 180.0/PI);}
	} else {
		printf("ERROR: division by zero in GetRotation\n");
	}

	// adjust for going over range
	if(b_tagup == true)
		b_val += 90.0;
	if(b_tagdown == true)
		b_val -= 90.0;
	if(c_tagup == true)
		c_val += 90.0;
	if(c_tagdown == true)
		c_val -= 90.0;
	if(d_tagup == true)
		d_val += 90.0;
	if(d_tagdown == true)
		d_val -= 90.0;

	ret.SetCoords(b_val, c_val, d_val);
	return ret;
}

void matrix4::PrintMatrix(void) const{  // this is just for testing purposes
	printf("[\n");
	for (int i = 0; i <=3; i += 1){
		printf("{");
		for (int j = 0; j <=3; j++){
			printf("%f ", elt[i][j]);	
		}
		printf("}\n");
	}
	printf("]\n");
}

// get the translational value.  This is going to be done by throwing away the top 3x3 matrix and
// sending back the translational values.       ADDED BY JONATHAN TOMPSON,  Tested and passed: 10/23/'05


coords3	matrix4::GetTranslation(void) const{ // CHANGED FOR PSET 3...
	coords3 ret;	// this will be the return value

	// find the values for ret and return it
	ret.SetCoords(elt[0][3], elt[1][3], elt[2][3]);
	return ret;
}


// compute the inverse of an affine matrix -- this assumes
// that the last row is [0, 0, 0, 1].
// bError is an optional boolean to flag an error -- for instance,
// if the matrix is not affine or if the inverse does not exist.
matrix4 matrix4::GetAffineInverse(bool *bError) const
{
	// output gets initialized to identity
	matrix4 ret;

	// start with no error
	if(bError)
		*bError = false;
	
	// check that the matrix is indeed affine
	if( (elt[3][0] != 0.0) || (elt[3][1] != 0.0) || (elt[3][2] != 0.0) || (elt[3][3] != 1.0) )
	{
		if(bError)
			*bError = true;

		// return the zero matrix, which is clearly not a unit
		ret.elt[0][0] = 0.0;
		ret.elt[1][1] = 0.0;
		ret.elt[2][2] = 0.0;
		ret.elt[3][3] = 0.0;
		printf("GetAffineInverse was given something other than an affine matrix!!!\n");
		return ret;
	}

	// now compute the determinant; we use the fact that the last row has
	// all zeros except one 1 in the w position.
	// therefore, we need only look at the upper left 3x3 minor and calculate
	// its determinant.  intuitively, we know translations are invertible, so
	// we're looking to see whether the _linear_ part is invertible.
	// Note: idea for expanding the products and using accumulation values
	// for positive and negative values was taken from the code for
	// Harvard's CS 175 class.
	double det;
	double pos, neg, temp;

#define ACCUMULATE    \
	if (temp >= 0.0)  \
	    pos += temp;  \
	else              \
	    neg += temp;

	static const double PRECISION_LIMIT = (1.0e-15);

	pos = neg = 0.0;
	temp =  elt[0][0] * elt[1][1] * elt[2][2];
	ACCUMULATE;	// semicolon not needed, but looked funny without
	temp = - elt[0][0] * elt[1][2] * elt[2][1];
	ACCUMULATE;
	temp = - elt[0][1] * elt[1][0] * elt[2][2];
	ACCUMULATE;
	temp =  elt[0][1] * elt[1][2] * elt[2][0];
	ACCUMULATE;
	temp =  elt[0][2] * elt[1][0] * elt[2][1];
	ACCUMULATE;
	temp = - elt[0][2] * elt[1][1] * elt[2][0];
	ACCUMULATE;
	det = pos + neg;

	// check for singular matrix
	if( (det == 0.0) || ( fabs(det/(pos-neg)) < PRECISION_LIMIT ) )
	{
		if(bError)
			*bError = true;

		// return the zero matrix, which is clearly not a unit
		ret.elt[0][0] = 0.0;
		ret.elt[1][1] = 0.0;
		ret.elt[2][2] = 0.0;
		ret.elt[3][3] = 0.0;

		return ret;
	}
	
	// ok, now return the inverse which is the matrix adjoint divided by
	// the determinant ('adjoint' here is different from that related
	// to dual spaces).  Adjoint here means: the (i,j) position is given
	// by the (j,i) cofactor of the matrix.
	
	// although we could reuse the variable 'det', it might be a bit
	// unclear to use 'det' its reciprocal value.
	double det_inv = 1.0 / det;

    ret.elt[0][0] =   ( elt[1][1] * elt[2][2] - elt[1][2] * elt[2][1] ) * det_inv;
    ret.elt[1][0] = - ( elt[1][0] * elt[2][2] - elt[1][2] * elt[2][0] ) * det_inv;
    ret.elt[2][0] =   ( elt[1][0] * elt[2][1] - elt[1][1] * elt[2][0] ) * det_inv;
    ret.elt[0][1] = - ( elt[0][1] * elt[2][2] - elt[0][2] * elt[2][1] ) * det_inv;
    ret.elt[1][1] =   ( elt[0][0] * elt[2][2] - elt[0][2] * elt[2][0] ) * det_inv;
    ret.elt[2][1] = - ( elt[0][0] * elt[2][1] - elt[0][1] * elt[2][0] ) * det_inv;
    ret.elt[0][2] =   ( elt[0][1] * elt[1][2] - elt[0][2] * elt[1][1] ) * det_inv;
    ret.elt[1][2] = - ( elt[0][0] * elt[1][2] - elt[0][2] * elt[1][0] ) * det_inv;
    ret.elt[2][2] =   ( elt[0][0] * elt[1][1] - elt[0][1] * elt[1][0] ) * det_inv;
    
	// invert the translation component(s)
	// (this is done by multiplying the translation (on the left) by the inverse linear map)
    ret.elt[0][3] = - ( elt[0][3] * ret.elt[0][0] + elt[1][3] * ret.elt[0][1] + elt[2][3] * ret.elt[0][2] );
    ret.elt[1][3] = - ( elt[0][3] * ret.elt[1][0] + elt[1][3] * ret.elt[1][1] + elt[2][3] * ret.elt[1][2] );
    ret.elt[2][3] = - ( elt[0][3] * ret.elt[2][0] + elt[1][3] * ret.elt[2][1] + elt[2][3] * ret.elt[2][2] );
    
    // inverse matrix is again affine
    ret.elt[3][0] = ret.elt[3][1] = ret.elt[3][2] = 0.0;
    ret.elt[3][3] = 1.0;

	return ret;
}

matrix4	matrix4::GetTranspose(void) const
{
	return matrix4(elt[0][0], elt[1][0], elt[2][0], elt[3][0],
					 elt[0][1], elt[1][1], elt[2][1], elt[3][1],
					 elt[0][2], elt[1][2], elt[2][2], elt[3][2],
					 elt[0][3], elt[1][3], elt[2][3], elt[3][3] );
}

matrix4	matrix4::GetNormalMatrix(void) const
{
	// return the inverse transpose of the linear part
	matrix4 inv(GetAffineInverse());
	inv.elt[0][3] = 0.0;
	inv.elt[1][3] = 0.0;
	inv.elt[2][3] = 0.0;
	return inv.GetTranspose();
}

void matrix4::Mat2GL(GLdouble *array) const
{
	// OpenGL stores the values by column.
	// unroll the loop manually.
	array[ 0] = elt[0][0];
	array[ 1] = elt[1][0];
	array[ 2] = elt[2][0];
	array[ 3] = elt[3][0];
	array[ 4] = elt[0][1];
	array[ 5] = elt[1][1];
	array[ 6] = elt[2][1];
	array[ 7] = elt[3][1];
	array[ 8] = elt[0][2];
	array[ 9] = elt[1][2];
	array[10] = elt[2][2];
	array[11] = elt[3][2];
	array[12] = elt[0][3];
	array[13] = elt[1][3];
	array[14] = elt[2][3];
	array[15] = elt[3][3];
}

void matrix4::Mat2GL(GLfloat *array) const
{
	// OpenGL stores the values by column.
	// unroll the loop manually.
	// (let the compiler handle type conversion)
	array[ 0] = elt[0][0];
	array[ 1] = elt[1][0];
	array[ 2] = elt[2][0];
	array[ 3] = elt[3][0];
	array[ 4] = elt[0][1];
	array[ 5] = elt[1][1];
	array[ 6] = elt[2][1];
	array[ 7] = elt[3][1];
	array[ 8] = elt[0][2];
	array[ 9] = elt[1][2];
	array[10] = elt[2][2];
	array[11] = elt[3][2];
	array[12] = elt[0][3];
	array[13] = elt[1][3];
	array[14] = elt[2][3];
	array[15] = elt[3][3];
}


matrix4& matrix4::operator += (const matrix4 &rhs)
{
	elt[0][0] += rhs.elt[0][0];
	elt[1][0] += rhs.elt[1][0];
	elt[2][0] += rhs.elt[2][0];
	elt[3][0] += rhs.elt[3][0];

	elt[0][1] += rhs.elt[0][1];
	elt[1][1] += rhs.elt[1][1];
	elt[2][1] += rhs.elt[2][1];
	elt[3][1] += rhs.elt[3][1];

	elt[0][2] += rhs.elt[0][2];
	elt[1][2] += rhs.elt[1][2];
	elt[2][2] += rhs.elt[2][2];
	elt[3][2] += rhs.elt[3][2];

	elt[0][3] += rhs.elt[0][3];
	elt[1][3] += rhs.elt[1][3];
	elt[2][3] += rhs.elt[2][3];
	elt[3][3] += rhs.elt[3][3];

	return *this;
}

matrix4& matrix4::operator -= (const matrix4 &rhs)
{
	elt[0][0] -= rhs.elt[0][0];
	elt[1][0] -= rhs.elt[1][0];
	elt[2][0] -= rhs.elt[2][0];
	elt[3][0] -= rhs.elt[3][0];

	elt[0][1] -= rhs.elt[0][1];
	elt[1][1] -= rhs.elt[1][1];
	elt[2][1] -= rhs.elt[2][1];
	elt[3][1] -= rhs.elt[3][1];

	elt[0][2] -= rhs.elt[0][2];
	elt[1][2] -= rhs.elt[1][2];
	elt[2][2] -= rhs.elt[2][2];
	elt[3][2] -= rhs.elt[3][2];

	elt[0][3] -= rhs.elt[0][3];
	elt[1][3] -= rhs.elt[1][3];
	elt[2][3] -= rhs.elt[2][3];
	elt[3][3] -= rhs.elt[3][3];

	return *this;
}

matrix4& matrix4::operator *= (double scale)
{
	elt[0][0] *= scale;
	elt[1][0] *= scale;
	elt[2][0] *= scale;
	elt[3][0] *= scale;

	elt[0][1] *= scale;
	elt[1][1] *= scale;
	elt[2][1] *= scale;
	elt[3][1] *= scale;

	elt[0][2] *= scale;
	elt[1][2] *= scale;
	elt[2][2] *= scale;
	elt[3][2] *= scale;

	elt[0][3] *= scale;
	elt[1][3] *= scale;
	elt[2][3] *= scale;
	elt[3][3] *= scale;

	return *this;
}


// F U N C T I O N S ///////////////////////////////////////////////////
// THESE NEXT FUNCTIONS ARE TO HELP US FIND ONLY THE ROTATION OR TRANSLATION MATRICIES 
// IN A 4X4 AFINE MATRIX
matrix4	GetTranslation(const matrix4 input) {
	return matrix4(1.0,		0.0,	0.0,	input.elt[0][3],  
				   0.0,		1.0,	0.0,    input.elt[1][3],  
				   0.0,		0.0,	1.0,    input.elt[2][3],
				   0.0,		0.0,	0.0,    1.0);
}

matrix4 GetRotation(const matrix4 input) {
	return matrix4(input.elt[0][0],		input.elt[0][1],	input.elt[0][2],	0.0,  
				   input.elt[1][0],		input.elt[1][1],	input.elt[1][2],	0.0,  
				   input.elt[2][0],		input.elt[2][1],	input.elt[2][2],	0.0,
				   0.0,					0.0,				0.0,				1.0);
}



matrix4 MakeXRotation(double deg)
{	
	double cosval, sinval;
	
	cosval = cos( deg * PI/180.0  );
	sinval = sin( deg * PI/180.0  );
	
	return matrix4(1.0,     0.0,     0.0,     0.0,  // fix the x-coordinate
				   0.0,  cosval,  sinval,     0.0,  // rotate y-z plane
				   0.0, -sinval,  cosval,     0.0,
				   0.0,     0.0,     0.0,     1.0); // fix the w-coordinate
}
	
matrix4 MakeYRotation(double deg)
{	
	double cosval, sinval;
	
	cosval = cos( deg * PI/180.0  );
	sinval = sin( deg * PI/180.0  );

	return matrix4( cosval,     0.0, -sinval,     0.0,  // rotate z-x plane
				       0.0,     1.0,     0.0,     0.0,  // fix y-axis
				    sinval,     0.0,  cosval,     0.0,
				       0.0,     0.0,     0.0,     1.0); // fix the w-coordinate	
}

matrix4 MakeZRotation(double deg)
{	
	double cosval, sinval;
	
	cosval = cos( deg * PI/180.0  );
	sinval = sin( deg * PI/180.0  );
	
	return matrix4( cosval,  sinval,     0.0,     0.0,  // rotate x-y plane
				   -sinval,  cosval,     0.0,     0.0,  
				       0.0,     0.0,     1.0,     0.0,  // fix the z-axis
				       0.0,     0.0,     0.0,     1.0); // fix the w-coordinate	
}

matrix4 MakeTranslation(double tx, double ty, double tz)
{
	return matrix4( 1.0, 0.0, 0.0,  tx,  // x translation
				    0.0, 1.0, 0.0,  ty,  // y translation
				    0.0, 0.0, 1.0,  tz,  // z translation
				    0.0, 0.0, 0.0, 1.0); // preserve point/vector	
}

matrix4 MakeScale(double sx, double sy, double sz)
{
	return matrix4(  sx, 0.0, 0.0, 0.0,  // x scale
				    0.0,  sy, 0.0, 0.0,  // y scale
				    0.0, 0.0,  sz, 0.0,  // z scale
				    0.0, 0.0, 0.0, 1.0); // preserve point/vector
}


matrix4 MakeProjection(double top, double bot, 
					   double left, double right,
					   double nval, double fval)
{
	matrix4 ret;

	double tmb = top - bot;
	double rml = right - left;
	double fmn = fval - nval;

	// first row:
	if(rml != 0.0) {
		double rml_inv = 1.0 / rml;
		ret.elt[0][0] = -2.0 * nval * rml_inv;
		ret.elt[0][1] = 0.0;
		ret.elt[0][2] = (right+left) * rml_inv;
		ret.elt[0][3] = 0.0;
	}
	else
		ret.elt[0][0] = ret.elt[0][1] = ret.elt[0][2] = ret.elt[0][3] = 0.0;

	// second row:
	if(tmb != 0.0) {
		double tmb_inv = 1.0 / tmb;
		ret.elt[1][0] = 0.0;
		ret.elt[1][1] = -2.0 * nval * tmb_inv;
		ret.elt[1][2] = (top + bot) * tmb_inv;
		ret.elt[1][3] = 0.0;
	}
	else
		ret.elt[1][0] = ret.elt[1][1] = ret.elt[1][2] = ret.elt[1][3] = 0.0;

	// third row:
	if(fmn != 0.0) {
		double fmn_inv = 1.0 / fmn;
		ret.elt[2][0] = 0.0;
		ret.elt[2][1] = 0.0;
		ret.elt[2][2] = -(fval+nval) * fmn_inv;
		ret.elt[2][3] = 2.0 * fval * nval * fmn_inv;
	}
	else
		ret.elt[2][0] = ret.elt[2][1] = ret.elt[2][2] = ret.elt[2][3] = 0.0;

	ret.elt[3][0] = ret.elt[3][1] = ret.elt[3][3] = 0.0;
	ret.elt[3][2] = -1.0;

	return ret;
}


matrix4 MakeProjection(double fovy, double ar,
					   double zNear, double zFar)
{
	matrix4 ret;

	double fmn = -(zFar - zNear);
	double angle = fovy * 0.5 * PI / 180.0;
	double ca = cos(angle);
	double sa = sin(angle);
	double f;
	if(sa == 0.0)
		f = 0.0;
	else
		f = ca / sa;

	// first row:
	if(ar != 0.0) {
		ret.elt[0][0] = f / ar;
		ret.elt[0][1] = 0.0;
		ret.elt[0][2] = 0.0;
		ret.elt[0][3] = 0.0;
	}
	else
		ret.elt[0][0] = ret.elt[0][1] = ret.elt[0][2] = ret.elt[0][3] = 0.0;

	// second row:
	ret.elt[1][0] = 0.0;
	ret.elt[1][1] = f;
	ret.elt[1][2] = 0.0;
	ret.elt[1][3] = 0.0;

	// third row:
	if(fmn != 0.0) {
		double fmn_inv = 1.0 / fmn;
		ret.elt[2][0] = 0.0;
		ret.elt[2][1] = 0.0;
		ret.elt[2][2] = -(zFar+zNear) * fmn_inv;
		ret.elt[2][3] = 2.0 * zFar * zNear * fmn_inv;
	}
	else
		ret.elt[2][0] = ret.elt[2][1] = ret.elt[2][2] = ret.elt[2][3] = 0.0;

	ret.elt[3][0] = ret.elt[3][1] = ret.elt[3][3] = 0.0;
	ret.elt[3][2] = -1.0;

	return ret;
}

