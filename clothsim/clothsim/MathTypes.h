//////////////////////////////////////////////////////////////
//															//
//		MathTypes.h											//
//		By Hamilton Chong									//
//		(c) 2004											//
//															//
//		Header for the different math objects used			//
//															//
//////////////////////////////////////////////////////////////
//															//
//															//
//      Note: Added for use in Jonathan T and Andre M's     //
//			  Final Project	(1/13/2006)						//
//															//
//		Many thanks to Hamilton for supplying the code		//
//															//
//															//
//////////////////////////////////////////////////////////////


#ifndef MATHTYPES_H
#define MATHTYPES_H

#include <vector>

class vector3d;

///////////////////////////////////////////////////////////
// Matrix3x3 -- 3x3 block that is not necessarily sparse //
///////////////////////////////////////////////////////////
class Matrix3x3
{
public:
	double		elt[9];		// store as 1D array; usually has less overhead than 2D arrays
public:
	Matrix3x3() {for(int i=0;i<9;i++) elt[i]=0.0;}
	Matrix3x3(const Matrix3x3 &src) {for(int i=0;i<9;i++) elt[i]=src.elt[i];}
	~Matrix3x3(){};

	Matrix3x3& operator=( const Matrix3x3 &src );
	Matrix3x3& operator+=( const Matrix3x3 &src );
	Matrix3x3& operator-=( const Matrix3x3 &src );
	Matrix3x3& operator*=( double scale );
	// standard matrix multiply
	vector3d operator*( const vector3d &src );
	void Zero(void);
	void Ident(void);
};

inline const Matrix3x3 operator +(const Matrix3x3 &lhs, const Matrix3x3 &rhs)
{
	return Matrix3x3(lhs) += rhs;
}

inline const Matrix3x3 operator -(const Matrix3x3 &lhs, const Matrix3x3 &rhs)
{
	return Matrix3x3(lhs) -= rhs;
}

inline const Matrix3x3 operator *(const Matrix3x3 &lhs, double rhs)
{
	return Matrix3x3(lhs) *= rhs;
}

inline const Matrix3x3 operator *(double lhs, const Matrix3x3 &rhs)
{
	return Matrix3x3(rhs) *= lhs;
}

///////////////////////////////////////////////
// vector3d -- a standard 3-component vector //
///////////////////////////////////////////////
class vector3d
{
public:
	double x, y, z;
public:
	vector3d():x(0),y(0),z(0) {}
	vector3d(double vx, double vy, double vz):x(vx),y(vy),z(vz) {}
	vector3d(const vector3d &src):x(src.x),y(src.y),z(src.z) {}
	~vector3d(){};

	// meant for convenience; don't abuse -- use parantheses to ensure precedence
	vector3d& operator=( const vector3d &src );
	vector3d& operator+=( const vector3d &src );
	vector3d& operator-=( const vector3d &src );
	vector3d& operator*=( double scale );
	vector3d& operator/=( double scale );
	double operator*( const vector3d &src);

	// Turn vector into 3x3 mat by V*V_transpose
	Matrix3x3 CreateMat(void);
	Matrix3x3 CreateMat(const vector3d &src);
	
	////  ********* T H E S E   F U N C T I O N S   A D D E D   B Y   J O N N O *********
	vector3d CrossProduct(vector3d &src);
	void Normalize(void);

};

inline const vector3d operator +(const vector3d &lhs, const vector3d &rhs)
{
	return vector3d(lhs) += rhs;
}

inline const vector3d operator -(const vector3d &lhs, const vector3d &rhs)
{
	return vector3d(lhs) -= rhs;
}

inline const vector3d operator *(const vector3d &lhs, double rhs)
{
	return vector3d(lhs) *= rhs;
}

inline const vector3d operator *(double lhs, const vector3d &rhs)
{
	return vector3d(rhs) *= lhs;
}


////////////////////////////////////////////////////////
// Diagonal Matrix -- special case of a sparse matrix //
////////////////////////////////////////////////////////
class DiagonalMatrix
{
public:
	int m_iSize;				// number of blocks
	vector3d*	m_block;		// 3 entries per block

public:
	DiagonalMatrix( int iSize=1, bool tmpblock= false );
//	DiagonalMatrix( const DiagonalMatrix &src );  // replaced with makeTemp(), to avoid allocating excess memory
	~DiagonalMatrix();

	// again, for convenience. remember that certain operations may be 
	//   expensive albeit seemingly innocuous to code (not such a big problem
	//   because matrices are diagonal)
	DiagonalMatrix& operator=( const DiagonalMatrix &src );
	void Zero();
	DiagonalMatrix& operator+=( const DiagonalMatrix &src );
	DiagonalMatrix& operator-=( const DiagonalMatrix &src );
	DiagonalMatrix& operator*=( double scale );

	bool Mult( DiagonalMatrix &src , DiagonalMatrix &dst );
	bool well_defined_op(DiagonalMatrix &m1, DiagonalMatrix &m2);

	bool Invert( DiagonalMatrix &ret );
	double DotProductDiagonals( const DiagonalMatrix &src );


	DiagonalMatrix*				temp;					// this is used in many operators;
														// it avoids over allocation of memory
	bool						tempblock;
	vector3d*					tempvecblock;

	// **********   ADDED BY JONNO   ***********
	void MakeTemp(void);

};

/// THESE OPERATORS WERE OLD CODE THAT MADE NEW MEM BLOCKS ALL THE TIME!!
/// EVENTUALLY OVERLOADING THE PAGEFILE!

/*inline DiagonalMatrix operator +(const DiagonalMatrix &lhs, const DiagonalMatrix &rhs)
{
	return DiagonalMatrix(lhs) += rhs;
}

inline const DiagonalMatrix operator -(const DiagonalMatrix &lhs, const DiagonalMatrix &rhs)
{
	return DiagonalMatrix(lhs) -= rhs;
}

inline const DiagonalMatrix operator *(const DiagonalMatrix &lhs, double rhs)
{
	return DiagonalMatrix(lhs) *= rhs;
}

inline const DiagonalMatrix operator *(double lhs, const DiagonalMatrix &rhs)
{
	return DiagonalMatrix(rhs) *= lhs;
}*/

//////////////////////////////////////////////////////////////////////////
// SymMatrixBlocks -- sparse matrix with symmetric 3x3 matrix blocks	//
//////////////////////////////////////////////////////////////////////////
// usually such matrices are composed of sub-blocks of 3x3 matrices.
// sparcity refers to the fact that many of these sub-blocks are all zero.
// however, in many cases, the nonzero sub-blocks are not themselves 
//   sparse (as in the case of cloth simulation).
// Furthermore, the matrix is symmetric, so we only need to store upper triangular
//   supermatrix
// Inspiration for using such an implementation comes from Dean Macri's implementation.
// Dean Macri is Senior Technical Marketing Engineer with Intel’s Developer Relations Division.
class SymMatrixBlocks
{
public:
	int							m_iSize;		// size of supermatrix w/ sub-blocks as unit
	int							*m_rowstart;	// beginning of row
	std::vector<int>			m_col_lookup;	// indicies to nonzero blocks (ordered)
	std::vector<Matrix3x3 *>	m_matBlock;		// nonzero blocks of data
	bool						m_bAllZero;		// all entries zero still?
	SymMatrixBlocks*			temp;			// this is used in many operators;
												// it avoids over allocation of memory
	bool						tempblock;
	Matrix3x3*					tempmatblock;



public:
	SymMatrixBlocks(int size, bool tmpblock);
//	SymMatrixBlocks( const SymMatrixBlocks &src );  // replaced with makeTemp(), to avoid allocating excess memory
	~SymMatrixBlocks();

	void Zero(void);							// zero the matrix

	SymMatrixBlocks& operator=( const SymMatrixBlocks &src );
	SymMatrixBlocks& operator+=( const SymMatrixBlocks &src );
	SymMatrixBlocks& operator-=( const SymMatrixBlocks &src );
	SymMatrixBlocks& operator*=( double scale );

	void SetDiag(DiagonalMatrix &src, DiagonalMatrix &ret);

	Matrix3x3* operator() (int row, int col);	// index overloading -- also allocates space

	// **********   ADDED BY JONNO   ***********
	void MakeTemp(void);
	
};


/// ********** CHANGED BY JONNO *************


/// THESE OPERATORS WERE OLD CODE THAT MADE NEW MEM BLOCKS ALL THE TIME!!
/// EVENTUALLY OVERLOADING THE PAGEFILE!
/*
inline const SymMatrixBlocks operator +(SymMatrixBlocks &lhs, const SymMatrixBlocks &rhs)
{
	return SymMatrixBlocks(lhs) += rhs;
}

inline SymMatrixBlocks operator -(SymMatrixBlocks &lhs, const SymMatrixBlocks &rhs)
{
	return SymMatrixBlocks(lhs) -= rhs;
}

inline const SymMatrixBlocks operator *(SymMatrixBlocks &lhs, double rhs)
{
	return SymMatrixBlocks(lhs) *= rhs;
}

inline SymMatrixBlocks operator *(double lhs, SymMatrixBlocks &rhs)
{
	return SymMatrixBlocks(rhs) *= lhs;
} */

/// *****************************************

#endif