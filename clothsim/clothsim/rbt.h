/////////////////////////////////////////////////////////////////////////////
//
//	rbt.h
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	Rigid body transformation class.
//
//	revision history:
//	Oct 26, 2005		Code by Hamilton Chong
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef	RBT_H
#define RBT_H

#include <glut.h>
#include "coords3.h"
#include "matrix4.h"
#include "qrot.h"

//////////////////////////////////////////////////////////
// rbt													//
// ---------------------------------------------------- //
// class for handling rigid body transformations		//
//														//
//////////////////////////////////////////////////////////

class rbt
{
public:
	coords3 translation;	// translation
	qrot	rotation;		// quaternion rotation

public:
	rbt() : translation(0.0, 0.0, 0.0), rotation() {}
	rbt(const rbt& motion) : translation(motion.translation), 
							 rotation(motion.rotation) {}
	rbt(const coords3& t, const qrot& r) : translation(t), rotation(r) {}
	
	// TODO: complete these constructors so that you can convert
	// pure translations and rotations into rigid body transformations
	// NOTE: the above constructors can form a model for how you do this...
	//		basically we do this by setting one of the two variables and leaving
	//		the other one null
	rbt(const coords3& t) :		translation(t),
								rotation() {}	
	rbt(const qrot& r) :		translation(0.0, 0.0, 0.0), 
								rotation(r) {}

	~rbt() {}

	matrix4	GetMatrix(void)		const
	{
		// TODO: implement!
		// get the rotation matrix from the rotation qrot
		matrix4 rot = rotation.Q2mat();
		// now add the translation compontents
		rot.elt[0][3] = translation.x;
		rot.elt[1][3] = translation.y;
		rot.elt[2][3] = translation.z;
		return rot;
	}

	rbt	GetInverse(void)	const
	{
		// TODO: implement inverse
		// All we need to do here is:	1. get the translation and rotation components
		//								2. Find the inverses of those parts
		//								3. return the caller a new rigid body transform 
		//								   comprised of these inverses.
		//  Note: O^-1 = (RT)^-1 = (R^-1)(T^-1) = (r^-1)( (TR as affine inverse).translation)
		qrot	rot_inv		= rotation.GetInverse();
		matrix4 trans		= MakeTranslation(translation.x, translation.y, translation.z);
		trans				= trans * rotation.Q2mat();
		coords3 trans_inv	= (trans.GetAffineInverse()).GetTranslation();

		return (rbt(trans_inv, rot_inv));
	}
	
	void	Rbt2GL(GLdouble *array) { (GetMatrix()).Mat2GL(array); }
	void	Rbt2GL(GLfloat  *array) { (GetMatrix()).Mat2GL(array); }
};

inline const coords3 operator *(const rbt &lhs, const coords3 &rhs)
{
	// TODO: implement multiplication of rbt lhs and coordinate rhs
	// What we need to do is make trans. matricies out of the RHS and LHS
	// Multiply them and send back the new trans. coordinates
	// NOTE: for A1 * A2 = [t1, r1] * t2
	//					   outputt = t1 * trans_component_of[(r1 * t2)] where r1, t1 & t2 
	//																		are matrix4s

	/*
	matrix4 rhs_trans = MakeTranslation(rhs.x, rhs.y, rhs.z);
	coords3 ret = (lhs_trans * rhs_trans).GetTranslation();
	return (ret);*/

	matrix4 t1 = GetTranslation(lhs.GetMatrix());
	matrix4 t2 = MakeTranslation(rhs.x, rhs.y, rhs.z);
	qrot r1 = lhs.rotation;
	coords3 ret_trans = (t1 * GetTranslation(r1.Q2mat() * t2)).GetTranslation();

	return(ret_trans);
}
 
inline const rbt operator *(const rbt &lhs, const rbt &rhs)
{
	// TODO: implement multiplication (composition) of lhs and rhs
	// In this case is we need to make rot and translation matricies for LHS and RHS, then
	// we need to multiply them.  Note:	for O	= A1 * A2
	//											= [t1, r1] * [t2, r2]  
	//									outputr = r1 * r2  where r1 and r2 are qrot
	//									outputt = t1 * trans_component_of[(r1 * t2)] where r1, t1 & t2 
	//																				  are matrix4
	// STEP 1: get matricies and qrots
	matrix4 t1 = GetTranslation(lhs.GetMatrix());
	matrix4 t2 = GetTranslation(rhs.GetMatrix());
	qrot r1 = lhs.rotation;
	qrot r2 = rhs.rotation;

	// STEP 2: find return values
	qrot ret_qrot = r1 * r2;
	coords3 ret_trans = (t1 * GetTranslation(r1.Q2mat() * t2)).GetTranslation();
	
	// STEP 3: return the values
	return rbt(ret_trans, ret_qrot);
}

#endif
