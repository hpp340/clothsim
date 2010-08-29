#ifndef COORDS3_H // header guards
#define COORDS3_H

/////////////////////////////////////////////////////////////////////////////
//
//	coords3.h
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

const double PI = 3.1415926535898;
#define RAD2DEG 360./(2*PI)


//////////////////////////////////////////////////////////
// coords3												//
// ---------------------------------------------------- //
// class for handling 3D coordinates					//
//														//
//////////////////////////////////////////////////////////

class coords3
{
public:
	double	x,y,z;
public:
	coords3():x(0.0), y(0.0), z(0.0) {}
	coords3(double fx, double fy, double fz):x(fx),y(fy),z(fz) {}
	coords3(const coords3& in) : x(in.x), y(in.y), z(in.z) {}
	~coords3() {}

	void	 SetCoords(double fx, double fy, double fz) { x=fx; y=fy; z=fz; }
	coords3& operator += (const coords3 &rhs);
	coords3& operator -= (const coords3 &rhs);
	coords3& operator *= (double scale);

	// friendly functions
	friend coords3 CrossProduct(const coords3& p, const coords3& q);
	friend double DotProduct(const coords3& p, const coords3& q)
	{ return (p.x * q.x + p.y * q.y + p.z * q.z); }
};

inline const coords3 operator +(const coords3 &lhs, const coords3 &rhs)
{
	return coords3(lhs) += rhs;
}

inline const coords3 operator -(const coords3 &lhs, const coords3 &rhs)
{
	return coords3(lhs) -= rhs;
}

inline const coords3 operator *(const coords3 &lhs, double rhs)
{
	return coords3(lhs) *= rhs;
}

inline const coords3 operator *(double lhs, const coords3 &rhs)
{
	return coords3(rhs) *= lhs;
}


#endif
