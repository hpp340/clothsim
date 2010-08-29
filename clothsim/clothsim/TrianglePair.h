#ifndef TRIANGLEPAIR_H // header guards
#define TRIANGLEPAIR_H

// This is the TrianglePair class.

// ADDED 2006

// this class helps with creating and managing all the triangle pairs.
class TrianglePair
{
public:
	triangle * tri1;			// the index of this triangle
	triangle * tri2;			// the index of the other triangle
	int vert1;					// the vertex indicies of the adjoining side.
	int vert2;

public:
	void SetTri(triangle * tri_in1, triangle * tri_in2, int index1, int index2);	// so I can set the vertex pair in one line of code
	TrianglePair() {					// constructor function.
		tri1 = NULL;
		tri2 = NULL;
		vert1=-1;
		vert2=-1;
	}
};

#endif