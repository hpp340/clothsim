//#ifndef GLOBAL_H // header guards
//#define GLOBAL_H

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <glut.h>
#include "matrix4.h"
#include "shader.h"
#include "coords3.h"
#include "cloth.h"
#include "MathTypes.h"
#include "ppmio.h"
#include "arcball.h"
#include "conjugategradient.h"
#include "frame.h"
#include "videocode.h"
#include <iostream>
#include <string>
#include "glFuncs.h"

// SEE GLOBALS.cpp FOR FULL DESCRIPTIONS AND DECLARATIONS

extern const float	FRUST_FOVY;	
extern const float	FRUST_NEAR;	
extern const float	FRUST_FAR;		
extern const float	FLOOR_Y;		
extern const float	FLOOR_SIZE;		
extern const int	NUM_SHADERS;
extern const int	NUM_CLOTHS;
extern const float	SPHERE_X;
extern const float	SPHERE_Y;
extern const float	SPHERE_Z;
extern const float	SPHERERADIUS;
extern const bool	DRAWSPHERE;
extern const bool	SPHERECONTACT;
extern const float	DELTAX_TOLLERENCE;
extern const bool	EULERSTEP;
extern const float	XNATLENGTH ;		
extern const float	YNATLENGTH;		
extern const float	DIAGNATLENGTH;
extern const float	BENDDIAGNATLENGTH;
extern const float	KSTRETCH;	
extern const float	KSHEAR;		
extern const float	KBEND;			
extern const float	KDSTRETCH ;		
extern const float	KDSHEAR;			
extern const float	KDBEND;			
extern const float	GRAVITY;			
extern float		HSTEP; // HSTEP IS NOW DYNAMIC (ADAPTIVE)
extern const float	ERRORTOLERANCE;
extern bool			reducehstep;
extern int			g_width;
extern int			g_height;
extern bool			g_left_clicked;
extern bool			g_right_clicked;
extern int			g_lclick_x;
extern int			g_lclick_y;		
extern int			g_rclick_x;
extern int			g_rclick_y;		
extern rbt			g_eye_matrix;				
extern rbt			g_base_frame;				
extern rbt			g_sky_camera;
extern coords3		g_lightW;					
extern float		g_twidth;
extern float		g_theight;
extern bool			flooron;
extern bool			g_block;
extern cloth *		g_cloth;		
extern int			g_clothlength;		
extern int			g_clothheight;			
extern int			g_mass;					
extern float		g_XSize;			
extern float		g_YSize;
extern bool			g_drawball;		
//extern const int	matrixsize;		
//#define matrixsize 27; // Can't use extern const with array initializations
extern frame *		cur_frame;
extern GLuint		h_texture;			
extern GLuint		h_program;					
extern GLint		h_uProjMatrix;				
extern GLint		h_uModelViewMatrix;			
extern GLint		h_uNormalMatrix;	
extern GLuint		h_vTexCoord;				
extern GLuint		h_texUnit0;					
extern GLint		h_uLight;					
extern GLint		h_vColor;					
extern GLint		h_drawcloth;				
extern GLint		h_vMVIndex;					
extern bool			fullscreen;

//#endif