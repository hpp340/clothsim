#ifndef ARCBALL_H // header guards
#define ARCBALL_H
/////////////////////////////////////////////////////////////////////////////
//
//	arcball.h
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	arcball helper functions
//
//	revision history:
//	Nov 08, 2005		Code by Jonathan Tompson
//
/////////////////////////////////////////////////////////////////////////////
//
//
//			TAKEN FROM Jonathan's ASST3 for use with our Final Project
//
//
/////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <glut.h>
#include "ppmio.h"
#include "matrix4.h"
#include "shader.h"
#include "rbt.h"
#include "qrot.h"
#include "matrix4.h"
#include <assert.h>

void arcball (rbt &O_frame_in_eye, rbt &O_frame, rbt &new_O_frame, rbt &S_frame, rbt &inv_S_frame, 
			  int g_width, int g_height, double old_x, double old_y, double new_x, 
			  double new_y, int g_manip_object);

void GetScreenSpaceCircle(const coords3& center, double radius,
						  const matrix4& projection,			
						  unsigned int screen_width, unsigned int screen_height,
						  coords3& out_center, double& out_radius);	
#endif