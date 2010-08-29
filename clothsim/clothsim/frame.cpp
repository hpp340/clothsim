#include "StdAfx.h"
#include "frame.h"
#include "MathTypes.h"
#include "cloth.h"

frame::~frame(void)
{
}

void frame::SetFrame(DiagonalMatrix &pos) {
	for (int i = 0; i<numberverticies;i++) {
		POS->m_block[i].x = pos.m_block[i].x;
		POS->m_block[i].y = pos.m_block[i].y;
		POS->m_block[i].z = pos.m_block[i].z;
	}
} 

void frame::UpdateToFrame(cloth &ret) {
	for (int i = 0; i<numberverticies;i++) {
		ret.POS->m_block[i].x = POS->m_block[i].x;
		ret.POS->m_block[i].y = POS->m_block[i].y;
		ret.POS->m_block[i].z = POS->m_block[i].z;
	}
}