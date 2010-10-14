//////////////////////////////////////////////////////////////
//															//
//		MathTypes.cpp										//
//		By Hamilton Chong									//
//		(c) 2004											//
//															//
//		Source for the different math objects used			//
//															//
//////////////////////////////////////////////////////////////
//															//
//															//
//      Note: Added for use in Jonathan T and Andre M's     //
//			  Final Project	(1/13/2006)						//
//															//
//		Many thanks to Hamilton for supplying the code		//
//															//
//		Besides the two functions at the top, there are		//
//		a few places where we have altered the code.		//
//		Namely, to prevent over allocation of memory.		//
//      Also, much of the operator code was re-written		//
//															//
//////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MathTypes.h"
#include <math.h>

// vector operations

////  ********* T H E S E   F U N C T I O N S   A D D E D   B Y   J O N N O *********
vector3d vector3d::CrossProduct(vector3d &src) {
	vector3d ret;
	//for (ux, uy, uz)x(vx, vy, vz) = x_hat(uy.vz-uz.vy) + y_hat(uz.vx-ux.vz) + 
	//								  z_hat(ux.vy-uy.vx)	
	ret.x = ((y*src.z) - (z*src.y));
	ret.y = ((z*src.x) - (x*src.z));
	ret.z = ((x*src.y) - (y*src.x));
	return (ret);
}

void vector3d::Normalize (void) {
	double mag = sqrt ((x*x) + (y*y) + (z*z));
	x = x/mag;
	y = y/mag;
	z = z/mag;
}

void SymMatrixBlocks::MakeTemp(void) {
	// some dummy init just in case...
	int i;
	temp->m_col_lookup.clear();
	temp->m_matBlock.clear();
	temp->m_bAllZero = true;

	i = (int)m_matBlock.size();

	if (temp->tempmatblock == NULL) {
		temp->tempmatblock = new Matrix3x3[m_matBlock.size()];			// make a new (fresh) memory block 
	}

	// allocate for new information
	if(m_iSize>0)
	{
		temp->m_iSize = m_iSize;
		if (temp->m_rowstart == NULL) {
			temp->m_rowstart = new int[m_iSize+1];
		}

		// now copy over new information
		for(i=0;i<=m_iSize;i++)
			temp->m_rowstart[i] = m_rowstart[i];
		for(i=0;i<(int)m_col_lookup.size();i++)
			temp->m_col_lookup.push_back(m_col_lookup[i]);
		temp->m_bAllZero = m_bAllZero;
		for(i=0;i<(int)m_matBlock.size();i++)
		{
			(temp->tempmatblock[i]) = *(m_matBlock[i]);
			temp->m_matBlock.push_back(&temp->tempmatblock[i]);
		}

	}
	else
	{		// we shouldn't ever get here
		temp->m_iSize = 0;
		temp->m_rowstart = NULL;
		printf("ERROR: PASSED BAD MAKETEMP\n");
		exit(1);
	}
	return;
}

void DiagonalMatrix::MakeTemp(void)
{
	// check for errors
	if (temp == NULL) {
		printf("something bad has happened\n");
		exit(1);
	}

	// initialize the array
	if (temp->m_block == NULL)
		temp->m_block = new vector3d[m_iSize];
	temp->m_iSize = m_iSize;

	// copy the values over
	if( m_iSize > 0 )
		memcpy( temp->m_block, m_block, temp->m_iSize * sizeof( vector3d ) );
}


////   ******************************************************************************

vector3d& vector3d::operator=( const vector3d &src )
{
	if(&src == this)
		return *this;
	x = src.x;
	y = src.y;
	z = src.z;
	return *this;
}
vector3d& vector3d::operator+=( const vector3d &src )
{
	x += src.x;
	y += src.y;
	z += src.z;
	return *this;
}
vector3d& vector3d::operator-=( const vector3d &src )
{
	x -= src.x;
	y -= src.y;
	z -= src.z;
	return *this;
}
vector3d& vector3d::operator*=( double scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}
vector3d& vector3d::operator/=( double scale )
{
	if(scale!=0)
	{
		double oneoverscale = 1/scale;
		x *= oneoverscale;
		y *= oneoverscale;
		z *= oneoverscale;
	}
	return *this;
}
double vector3d::operator*(const vector3d &src)
{
	return x*src.x + y*src.y + z*src.z;
}

Matrix3x3 vector3d::CreateMat(void)
{
	Matrix3x3 ret;
	ret.elt[0] = x*x;
	ret.elt[1] = x*y;
	ret.elt[2] = x*z;
	ret.elt[3] = ret.elt[1];
	ret.elt[4] = y*y;
	ret.elt[5] = y*z;
	ret.elt[6] = ret.elt[2];
	ret.elt[7] = ret.elt[5];
	ret.elt[8] = z*z;
	return ret;
}
Matrix3x3 vector3d::CreateMat(const vector3d &src)
{
	Matrix3x3 ret;
	ret.elt[0] = x*src.x;
	ret.elt[1] = x*src.y;
	ret.elt[2] = x*src.z;
	ret.elt[3] = y*src.x;
	ret.elt[4] = y*src.y;
	ret.elt[5] = y*src.z;
	ret.elt[6] = z*src.x;
	ret.elt[7] = z*src.y;
	ret.elt[8] = z*src.z;
	return ret;
}

/**************************************************************/

Matrix3x3& Matrix3x3::operator=( const Matrix3x3 &src )
{
	if(&src == this)
		return *this;
	elt[0] = src.elt[0];
	elt[1] = src.elt[1];
	elt[2] = src.elt[2];
	elt[3] = src.elt[3];
	elt[4] = src.elt[4];
	elt[5] = src.elt[5];
	elt[6] = src.elt[6];
	elt[7] = src.elt[7];
	elt[8] = src.elt[8];
	return *this;
}
Matrix3x3& Matrix3x3::operator+=( const Matrix3x3 &src )
{
	elt[0] += src.elt[0];
	elt[1] += src.elt[1];
	elt[2] += src.elt[2];
	elt[3] += src.elt[3];
	elt[4] += src.elt[4];
	elt[5] += src.elt[5];
	elt[6] += src.elt[6];
	elt[7] += src.elt[7];
	elt[8] += src.elt[8];
	return *this;
}
Matrix3x3& Matrix3x3::operator-=( const Matrix3x3 &src )
{
	elt[0] -= src.elt[0];
	elt[1] -= src.elt[1];
	elt[2] -= src.elt[2];
	elt[3] -= src.elt[3];
	elt[4] -= src.elt[4];
	elt[5] -= src.elt[5];
	elt[6] -= src.elt[6];
	elt[7] -= src.elt[7];
	elt[8] -= src.elt[8];
	return *this;
}
vector3d Matrix3x3::operator*( const vector3d &src)
{
	vector3d ret;
	ret.x = elt[0]*src.x + elt[1]*src.y + elt[2]*src.z;
	ret.y = elt[3]*src.x + elt[4]*src.y + elt[5]*src.z;
	ret.z = elt[6]*src.x + elt[7]*src.y + elt[8]*src.z;
	return ret;
}
Matrix3x3& Matrix3x3::operator*=(double scale)
{
	elt[0] *= scale;
	elt[1] *= scale;
	elt[2] *= scale;
	elt[3] *= scale;
	elt[4] *= scale;
	elt[5] *= scale;
	elt[6] *= scale;
	elt[7] *= scale;
	elt[8] *= scale;
	return *this;
}
void Matrix3x3::Zero(void)
{
	elt[0] = 0;
	elt[1] = 0;
	elt[2] = 0;
	elt[3] = 0;
	elt[4] = 0;
	elt[5] = 0;
	elt[6] = 0;
	elt[7] = 0;
	elt[8] = 0;
}
void Matrix3x3::Ident(void)
{
	elt[0] = 1;
	elt[1] = 0;
	elt[2] = 0;
	elt[3] = 0;
	elt[4] = 1;
	elt[5] = 0;
	elt[6] = 0;
	elt[7] = 0;
	elt[8] = 1;
}

/**************************************************************/

DiagonalMatrix::DiagonalMatrix( int iSize, bool tmpblock ) //MODIFIED BY JONNO AND ANDRE!!
{
	tempblock = tmpblock;
	if(tempblock == false)
		temp = new DiagonalMatrix(iSize, true);

	tempvecblock = NULL;

	if( iSize > 0 )
	{
		m_iSize = iSize;
		m_block = new vector3d[m_iSize]; 
	}
	else
	{
		// this should never happen
		m_iSize = 0;
		m_block = NULL;
	}
}

/*		NO LONGER NEEDED REPLACED WITH MAKETEMP()
DiagonalMatrix::DiagonalMatrix( const DiagonalMatrix &src )
{
	// initialize the array
	m_block = new vector3d[src.m_iSize];
	m_iSize = src.m_iSize;

	// copy the values over
	if( src.m_iSize > 0 )
		memcpy( m_block, src.m_block, m_iSize * sizeof( vector3d ) );
}*/

DiagonalMatrix::~DiagonalMatrix()
{
	if( m_block )
	{
		delete [] m_block;
		m_block = NULL;
	}
}

DiagonalMatrix& DiagonalMatrix::operator=( const DiagonalMatrix &src )
{
	if( m_iSize != src.m_iSize )		// I've tested it and it never gets here
	{
		if( m_block )
			delete [] m_block;
		m_block = new vector3d[src.m_iSize];

		m_iSize = src.m_iSize;
	}
	if( src.m_iSize > 0 )
	{
		memcpy( m_block, src.m_block, m_iSize * sizeof( vector3d ) );
	}
	return *this;
}

void DiagonalMatrix::Zero()
{
	if( m_iSize && m_block )
	{
		// memset caused memory leaks!!
		//memset( m_block, 0, m_iSize * sizeof( vector3d ) );
		for(int i = 0; i < m_iSize; i ++)
		{ m_block[i].x = 0.0; m_block[i].y = 0.0; m_block[i].z = 0.0; }
	}
}

DiagonalMatrix& DiagonalMatrix::operator+=( const DiagonalMatrix &src )
{
	// ideally, these two diagonal matrices should be of the same size...
	//  but otherwise, there is a "natural" definition that will be allowed
	int minimum = m_iSize;
	if(src.m_iSize < minimum)
		minimum = src.m_iSize;
	for(int index=0;index<minimum;index++)
	{
		m_block[index] += src.m_block[index];
	}
	return *this;
}

DiagonalMatrix& DiagonalMatrix::operator-=( const DiagonalMatrix &src )
{
	// ideally, these two diagonal matrices should be of the same size...
	//  but otherwise, there is a "natural" definition that will be allowed
	int minimum = m_iSize;
	if(src.m_iSize < minimum)
		minimum = src.m_iSize;
	for(int index=0;index<minimum;index++)
	{
		m_block[index] -= src.m_block[index];
	}
	return *this;
}

DiagonalMatrix& DiagonalMatrix::operator*=( double scale )
{
	for(int index=0;index<m_iSize;index++)
		m_block[index] *= scale;
	return *this;
}

bool DiagonalMatrix::Mult( DiagonalMatrix &src , DiagonalMatrix &dst )
{
	if(dst.m_iSize != m_iSize) return false;
	for(int index=0;index<m_iSize;index++)
	{
		dst.m_block[index].x = (m_block[index].x * src.m_block[index].x);
		dst.m_block[index].y = (m_block[index].y * src.m_block[index].y);
		dst.m_block[index].z = (m_block[index].z * src.m_block[index].z);
	}
	return true;
}

// helper function for checking whether a matrix-matrix op would be 
//   well-defined in the usual sense; just checks sizes of matrices
bool well_defined_op(DiagonalMatrix &m1, DiagonalMatrix &m2)
{
	if(m1.m_iSize != m2.m_iSize )
		return false;
	return true;
}

bool DiagonalMatrix::Invert(DiagonalMatrix &ret)
{
	// inversion of a diagonal matrix is simply the inversion of each
	//   element. however, zero element entries make for a singular
	//   matrix and is therefore inversion is not well-defined. 
	// singular matrices will just return themselves as inverses
	// (user's responsibility to not invert singular matrices)
	if(ret.m_iSize != m_iSize) return false;
	bool nonzero = true;
	for( int i=0; i<m_iSize; i++ )
	{
		int bx = (int)m_block[i].x;
		int by = (int)m_block[i].y;
		int bz = (int)m_block[i].z;
		if((bx!=0)&&(by!=0)&&(bz!=0))
		{
			ret.m_block[i].x = 1.0 / bx;
			ret.m_block[i].y = 1.0 / by;
			ret.m_block[i].z = 1.0 / bz;
		}
		else
			nonzero = false;
	}
	if(!nonzero)
	{
		// make singular matrices return themselves
		return false;
	}
	return true;
}
// this treates each diagonal matrix as a vector and performs the dot product
double DiagonalMatrix::DotProductDiagonals( const DiagonalMatrix &src )
{
	// should be equal size, but if not, do best we can.
	int minimum = m_iSize;
	if(src.m_iSize<minimum) minimum = src.m_iSize;
	double ret = 0.0;

	// perform dot product on each vector element
	for( int i=0; i<minimum; i++ )
		ret += (m_block[i] * src.m_block[i]); 

	return ret;
}


/**************************************************************/

SymMatrixBlocks::SymMatrixBlocks(int size, bool tmpBlock)  //MODIFIED BY JONNO AND ANDRE!!
{
	tempblock = tmpBlock;
	if(tempblock == false)
		temp = new SymMatrixBlocks (size, true);
	else
		m_rowstart = NULL;

	tempmatblock = NULL;
	
	if(size>0)
	{
		m_iSize = size;
		// allocate one more than size so we can store the 
		//  "length" of each row the same way
		m_rowstart = new int[size+1];
		for(int i=0;i<=size;i++)
			m_rowstart[i] = 0;
	}
	else
	{
		m_iSize = 0;
		m_rowstart = NULL;
	}
	m_bAllZero = true;
}

/*		NO LONGER NEEDED REPLACED WITH MAKETEMP()
SymMatrixBlocks::SymMatrixBlocks( const SymMatrixBlocks &src )
{
	// some dummy init just in case...
	int i;
	m_iSize = 0;
	m_rowstart = NULL;
	m_col_lookup.clear();
	m_matBlock.clear();
	m_bAllZero = true;

	// allocate for new information
	if(src.m_iSize>0)
	{
		m_iSize = src.m_iSize;
		m_rowstart = new int[m_iSize+1];
		if(src.m_matBlock.size()>0) 
			m_bAllZero = false;

		// now copy over new information
		for(i=0;i<=m_iSize;i++)
			m_rowstart[i] = src.m_rowstart[i];
		for(i=0;i<src.m_col_lookup.size();i++)
			m_col_lookup.push_back(src.m_col_lookup[i]);
		m_bAllZero = src.m_bAllZero;
		for(i=0;i<src.m_matBlock.size();i++)
		{
			Matrix3x3 *temp = new Matrix3x3;
			(*temp) = *(src.m_matBlock[i]);
			m_matBlock.push_back(temp);
		}

	}
	else
	{
		m_iSize = 0;
		m_rowstart = NULL;
	}
}*/

SymMatrixBlocks::~SymMatrixBlocks()
{
	m_iSize = 0;
	if(m_rowstart)
	{
		delete [] m_rowstart;
		m_rowstart = NULL;
	}
	m_col_lookup.erase(m_col_lookup.begin(),m_col_lookup.end());
	for(int i=0;i<(int)m_matBlock.size();i++)
	{
		if(m_matBlock[i])
			delete m_matBlock[i];
		m_matBlock.erase(m_matBlock.begin(),m_matBlock.end());
	}
	m_bAllZero = true;
}

SymMatrixBlocks& SymMatrixBlocks::operator=( const SymMatrixBlocks &src )	//MODIFIED BY JONNO AND ANDRE!!
{
	int i;

	if (tempmatblock == NULL) {
		tempmatblock = new Matrix3x3[src.m_matBlock.size()];			// make a new (fresh) memory block 
	}

	// cleanup what's currently stored here
	m_iSize = 0;
	if(m_rowstart)
	{
		delete [] m_rowstart;
		m_rowstart = NULL;
	}
	m_col_lookup.erase(m_col_lookup.begin(),m_col_lookup.end());
	/*for(i=0;i<m_matBlock.size();i++)
	{
		if(m_matBlock[i])
			delete m_matBlock[i];
		m_matBlock.erase(m_matBlock.begin(),m_matBlock.end());
	}*/
	m_matBlock.clear();

	m_bAllZero = true;
	// allocate for new information
	if(src.m_iSize>0)
	{
		m_iSize = src.m_iSize;
		m_rowstart = new int[m_iSize+1];
		if(src.m_matBlock.size()>0) m_bAllZero = false;

		// now copy over new information
		for(i=0;i<=m_iSize;i++)
			m_rowstart[i] = src.m_rowstart[i];
		for(i=0;i<(int)src.m_col_lookup.size();i++)
			m_col_lookup.push_back(src.m_col_lookup[i]);
		m_bAllZero = src.m_bAllZero;
		for(i=0;i<(int)src.m_matBlock.size();i++)
		{
			(tempmatblock[i]) = *(src.m_matBlock[i]);
			m_matBlock.push_back(&tempmatblock[i]);
		}

	}
	else
	{
		m_iSize = 0;
		m_rowstart = NULL;
	}

	return *this;

}

void SymMatrixBlocks::Zero(void)
{
	int i;
	for(i=0;i<=m_iSize;i++)
		m_rowstart[i] = 0;
	m_col_lookup.erase(m_col_lookup.begin(),m_col_lookup.end());
	for(i=0;i<(int)m_matBlock.size();i++)
	{
		if(m_matBlock[i])
			delete m_matBlock[i];
	}
	m_matBlock.erase(m_matBlock.begin(),m_matBlock.end());
	m_bAllZero = true;
}

SymMatrixBlocks& SymMatrixBlocks::operator+=( const SymMatrixBlocks &src )
{
	// check that matrices have compatible sizes; if not, simply return this matrix
	if(m_iSize != src.m_iSize)
		return *this;

	// don't waste time looping through a zero matrix
	if(src.m_bAllZero)
		return *this;

	for(int i=0; i<src.m_iSize; i++ )
	{
		int start = src.m_rowstart[i];
		int length = src.m_rowstart[i+1] - start;
		for(int j = 0; j<length; j++ )
			(*((*this)(i, src.m_col_lookup[start+j] ))) += (*(src.m_matBlock[start+j]));
	}

	return *this;
}

SymMatrixBlocks& SymMatrixBlocks::operator-=( const SymMatrixBlocks &src )
{
	// check that matrices have compatible sizes; if not, simply return this matrix
	if(m_iSize != src.m_iSize)
		return *this;

	// don't waste time looping through a zero matrix
	if(src.m_bAllZero)
		return *this;

	for(int i=0; i<src.m_iSize; i++ )
	{
		int start = src.m_rowstart[i];
		int length = src.m_rowstart[i+1] - start;
		for(int j = 0; j<length; j++ )
			(*((*this)(i, src.m_col_lookup[start+j] ))) -= (*(src.m_matBlock[start+j]));
	}

	return *this;
}

SymMatrixBlocks& SymMatrixBlocks::operator*=( double scale )
{
	if(!m_bAllZero)
	{

		for(int i=0; i<m_iSize; i++ )
		{
			int start = m_rowstart[i];
			int length = m_rowstart[i+1] - start;
			for(int j = 0; j<length; j++ )
			{
				(*((*this)(i,m_col_lookup[start+j]))) *= scale;
			}
		}
	}

	return *this;
}

void SymMatrixBlocks::SetDiag(DiagonalMatrix &src, DiagonalMatrix &ret)
{
	// this function treats DiagonalMatrix src as a column vector with
	// the diagonal entries as the vector components.
	// the result is another diagonal matrix with the diagonal entries
	// treates as the components of the resulting vector from the 
	// matrix-vector multiply.

	ret.Zero();
	// these really must be of the same size to make sense.
	if( m_iSize != src.m_iSize)
		return;

	// if all entries are still zero, we're already done
	if(m_bAllZero) return;

	for(int i=0; i<m_iSize; i++ )
	{
		int start = m_rowstart[i];
		int length = m_rowstart[i+1] - start;
		for(int j = 0; j<length; j++ )
		{
			// perform multiplication for nonzero matrix blocks
			int iCol = m_col_lookup[start+j];
			Matrix3x3 *temp = m_matBlock[start+j];
			ret.m_block[i].x += temp->elt[0] * src.m_block[iCol].x + temp->elt[1] * src.m_block[iCol].y + temp->elt[2] * src.m_block[iCol].z;
			ret.m_block[i].y += temp->elt[3] * src.m_block[iCol].x + temp->elt[4] * src.m_block[iCol].y + temp->elt[5] * src.m_block[iCol].z;
			ret.m_block[i].z += temp->elt[6] * src.m_block[iCol].x + temp->elt[7] * src.m_block[iCol].y + temp->elt[8] * src.m_block[iCol].z;
			// since we stored stuff in upper triangular form, we need to use the transpose of the appropriate matrix block for the complementary matrix-vector mult
			// NOTE: symmetry does not hold for the supermatrix or the submatrices necessarily, but for the matrix obtained by embedding the submatrices into the supermatrix structure
			if( i != iCol )
			{
				ret.m_block[iCol].x += temp->elt[0] * src.m_block[i].x + temp->elt[3] * src.m_block[i].y + temp->elt[6] * src.m_block[i].z;
				ret.m_block[iCol].y += temp->elt[1] * src.m_block[i].x + temp->elt[4] * src.m_block[i].y + temp->elt[7] * src.m_block[i].z;
				ret.m_block[iCol].z += temp->elt[2] * src.m_block[i].x + temp->elt[5] * src.m_block[i].y + temp->elt[8] * src.m_block[i].z;
			}
		}
	}
}

Matrix3x3* SymMatrixBlocks::operator() (int row, int col)
{
	int i, start, l, length;

	m_bAllZero = false;

	// store using upper triangular matrix.
	// so if indexing lower blocks, reverse indexing to get transpose.
	if( row > col  )
	{
		start = row;
		row = col;
		col = start;
	}

	start = m_rowstart[row];
	//if(row+1<m_iSize)
	//	length = m_rowstart[row+1] - start;
	//else
	//	length = 0;
	length = m_rowstart[row+1] - start;
	// if entree already exists, find it.
	for( l=0; l<length; l++ )
		if( m_col_lookup[start+l] == col )
			break;
	// if found a pre-existing entree, return it
	if(  (length != 0) && (l != length) )
	{
		// return the sub-matrix pointer
		return m_matBlock[start+l];
	}
	else
	{
		// find appropriate place in row (keep in order although not necessary)
		for( l=0; l<length; l++ )
			if( m_col_lookup[start+l] >= col )
				break;

		m_col_lookup.insert(m_col_lookup.begin()+start+l, col);
		Matrix3x3 *tempmat = new Matrix3x3();
		m_matBlock.insert(m_matBlock.begin()+start+l, tempmat);

		for( i=row+1; i<=m_iSize; i++ )
			m_rowstart[i]++;

		return m_matBlock[start+l];
	}
}