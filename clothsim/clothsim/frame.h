#ifndef FRAME_H // header guards
#define FRAME_H

#pragma once
#include "cloth.h"
#include "MathTypes.h"

class frame
{
public:
	frame*				nextframe;
	frame*				lastframe;
	int					numberverticies;
	DiagonalMatrix*		POS;

	frame(int size) {	nextframe = NULL; 
						lastframe = NULL;
						numberverticies = size;
						POS = new DiagonalMatrix(size);};
public:
	void SetFrame(DiagonalMatrix &pos);
	void UpdateToFrame(cloth &ret);
	~frame(void);
};

#endif
