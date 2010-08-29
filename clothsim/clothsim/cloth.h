#ifndef CLOTH_H // header guards
#define CLOTH_H

#pragma once
#include "triangle.h"
#include "MathTypes.h"
#include <glew.h>
#include <glut.h>
#include "VertexPair.h"
#include "TrianglePair.h"
#include "coords3.h"
#include "rbt.h"

class cloth
{
public:
	// these are the values needed to perform the algorithm as in Baraff and Witkin's paper
	// NOTE: Diag Matricies are actually diagonal matricies with 3 unit vectors as diagonal entries
	DiagonalMatrix*		VEL;
	DiagonalMatrix*		POS;
	DiagonalMatrix*		MASS;			// mass matrix
	SymMatrixBlocks*	SMASS;			// symetric mass matrix
	DiagonalMatrix*		FORCETOTAL;
	SymMatrixBlocks*	TOTALdf_dx;		// in eq. 15
	SymMatrixBlocks*	TOTALdf_dv;		// in eq. 15
	DiagonalMatrix*		P;				// preconditioning
	DiagonalMatrix*		INV_P;
	DiagonalMatrix*		z;					// acelleration enforced in unconstrained direction (gravity)
	DiagonalMatrix*		s;					// line 12 in CG
	DiagonalMatrix*		r;					// residual in CG method
	DiagonalMatrix*		b;					// known CG method vector
	DiagonalMatrix*		m_y;				// ? In Hamilton's **********************
	DiagonalMatrix*		dv;					// This is what we're trying to solve in CG method	
	DiagonalMatrix*		constr_q;			// axis of constraint
	DiagonalMatrix*		constr_p;			// axis of constraint
	DiagonalMatrix*		c;					// line 5 in modified CG method
	SymMatrixBlocks*	A;					// in CG method eq. 16
	Matrix3x3*			S;					// constraint matrix array, see section 5.1
	VertexPair*			VertexPairArray;	// helps when we do forces
	TrianglePair*		TrianglePairArray;	// for bend forces
	DiagonalMatrix*		q;					// line 8 in CG
	DiagonalMatrix*		dmat;

	// very temp variables
	DiagonalMatrix*     temp;
	DiagonalMatrix*     temp2;
	SymMatrixBlocks*	symblocktemp;
	DiagonalMatrix*		normals;

	// GetFstretchOrShear temps
	vector3d *vel0, *vel1;
	vector3d *pos0, *pos1;
	float C;								// this is the vector condition used to find stretch force
	vector3d dC_dx[2];						// the dc_dx for each vertex
	float mag;
	float inv_mag;
	float inv_mag_cubed;
	float Cdot;
	vector3d force0, force1;				// the force contribution from stretch
	vector3d d2C_dxdx[2][2];				// temp second derivatives
	Matrix3x3 df_dx, df_dv;					// the force derivatives contribution from stretch
	Matrix3x3 mattmp;

	bool reducehstep;

	rbt spherecenter;
	float sphereradius;

	int height;
	int length;
	int numberverticies;
	int numbertriangles;				// a variable used in DrawCloth, set in the CreateTriangleArray
	int numbervertexpairs;
	int numbertrianglepairs;
	triangle *trianglelist;				//a list of triangles which will be referenced like an array.  
										//(for use in texture mapping)
	int max_iter;                       // maximum interations allowed for convergence in conjugate gradient
	int sucess;							// Number of sucessful steps without a Tstep reduction in a row.
	int hstep_increase_bound;			// If we increase Tstep and we fail on next frame, increase the time we wait for another increase.
	bool stepTooLarge;

public:
	cloth (int _length, int _height);						//constructor and destrictor functions
	~cloth (void) {};

	void CreateTriangleArray (void);		// creates the mesh of triangles
	void InitCloth(float xsize, float ysize, int mass);		// sets up initial conditions for the cloth
	void DrawCloth(GLuint & h_vTexCoord, bool block);	// obvious: draws the cloth
	// GetFStretchVertex - finds the F stretch value between two verticies (of index1 and 2).
	// it will add the force between those two verticies to the running total.
	// Note the totaldf_dx reference needed...  We need the address reference, it's just easier
	// to do it here.
	void GetFStretchOrShear (int index0, int index1, float natlength, float k, 
						    float kd, SymMatrixBlocks &totaldf_dx, SymMatrixBlocks &totaldf_dv);
	void GetFBend (TrianglePair *input, float k, float kd, SymMatrixBlocks &totaldf_dx, SymMatrixBlocks 
				   &totaldf_dv);
	void GetFGravity (int index0, float grav);
	void GetFTotal(float xnatlength, float ynatlength, float diagnatlength, float benddiagnatlength, 
					float kstretch, float kdstretch, float kshear, float kdshear, float kbend, 
					float kdbend, float grav);
	void CreateVertexPairs(void);
	void CreateTrianglePairs(void);
	void ResetForces(void);
	void SetS(void);
	void CheckConstraints(float x, float y, float z, float rad);
	void CreateConstraint3dof(int index);
	void CreateConstraint2dof(int index, vector3d p);
	void CreateConstraint1dof(int index, vector3d p, vector3d q);
	void CreateConstraint0dof(int index);
	void ConjugateGradient(float error, float g_hstep);
	void Euler(float g_hstep);
	bool checkStep(int numTries);
	void Filter(DiagonalMatrix &src, DiagonalMatrix &ret);

	void GetNormals(void);

	SymMatrixBlocks* Mult(double lhs, SymMatrixBlocks &rhs);
	SymMatrixBlocks* Mult(SymMatrixBlocks &lhs, double rhs);
	SymMatrixBlocks* Add(SymMatrixBlocks &lhs, SymMatrixBlocks &rhs);
	SymMatrixBlocks* Sub(SymMatrixBlocks &lhs, SymMatrixBlocks &rhs);

	DiagonalMatrix* Mult(double lhs, DiagonalMatrix &rhs);
	DiagonalMatrix* Mult(DiagonalMatrix &lhs, double rhs);
	DiagonalMatrix* Add(DiagonalMatrix &lhs, DiagonalMatrix &rhs);
	DiagonalMatrix* Sub(DiagonalMatrix &lhs, DiagonalMatrix &rhs);

};

#endif