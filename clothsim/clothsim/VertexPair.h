#ifndef VERTEXPAIR_H // header guards
#define VERTEXPAIR_H

// This is the VertexPair class.

// this class helps with creating and managing all the vertex pairs.
class VertexPair
{
public:
	int vert1;			// the index of this vertex
	int vert2;			// the index of the other vertex
	bool diag;
	bool horizontal;
	bool bend;
public:
	void SetVert(int input1, int input2, bool dia, bool horiz, bool ben);	// so I can set the vertex pair in one line of code
	VertexPair() {					// constructor function.
		vert1=-1;
		vert2=-1;
		diag = false;
		horizontal = true;
		bend = false;
	}
};

#endif