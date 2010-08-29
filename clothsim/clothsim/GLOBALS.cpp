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
#include "GLOBALS.h"
#include <iostream>
#include <string>
#include "glFuncs.h"

// G L O B A L S ///////////////////////////////////////////////////////

// constants
const float	FRUST_FOVY = 70.0;		/// 15 degree field of view in y direction
const float	FRUST_NEAR = -0.1;		/// near plane
const float	FRUST_FAR  = -50.0;		/// far plane
const float	FLOOR_Y	   = -5.5;		/// y coordinate of the floor
const float	FLOOR_SIZE = 5.0;		/// half the floor length
const int	NUM_SHADERS = 1;
const int	NUM_CLOTHS = 1;

const float	SPHERE_X = 0;
const float	SPHERE_Y = -1.0;
const float	SPHERE_Z = 0;
const float	SPHERERADIUS = 0.75;
const bool	DRAWSPHERE = false;
const bool	SPHERECONTACT = false;

const float	DELTAX_TOLLERENCE = (float)1e-1;
const bool	EULERSTEP = false;
const float	XNATLENGTH = 0.36;		/// the natural rest length of the cloth (same as g_size, ie at rest to start)
const float	YNATLENGTH = 0.36;		/// same but in y direction
const float	DIAGNATLENGTH = sqrt ((XNATLENGTH*XNATLENGTH)+(YNATLENGTH*YNATLENGTH));
const float	BENDDIAGNATLENGTH = (2 * DIAGNATLENGTH);
const float	KSTRETCH = 12000;		/// it should be large according to Baraff and Witkin 12k good value
const float	KSHEAR = 1000;			/// 1k is a good value
const float	KBEND = 500;			/// 500 is a good value
const float	KDSTRETCH = 1000;		/// dampening 1k is a good value if kstretch is 12k
const float	KDSHEAR = 600;			/// 0.6k is good value if kshear is 1k
const float	KDBEND = 500;			/// 500 is a good value if kbend is 500
const float	GRAVITY = 9.5;			/// As in: 9.8 m/s^2
// HSTEP IS NOW DYNAMIC (ADAPTIVE) --> This is just a starting value
float		HSTEP = 0.2;			/// The actual step size...  Keep it small (0.0085 default)
const float	ERRORTOLERANCE = (float)1e-1;

static bool	reducehstep = false;

//// OPEN GL application globals
int			g_width				= 1000;	/// screen width (previously 1680)
int			g_height			= 700;	/// screen height  (previously 1050)
bool		g_left_clicked		= false;/// is the left mouse button down?
bool		g_right_clicked		= false;/// is the right mouse button down?
int			g_lclick_x, g_lclick_y;		/// coordinates for mouse click event
int			g_rclick_x, g_rclick_y;		/// coordinates for mouse click event
rbt			g_eye_matrix;				/// rigid transform describing eye position and orientation
rbt			g_base_frame;				/// rotation of base
rbt			g_sky_camera;
coords3		g_lightW;					/// light position		
float		g_twidth;
float		g_theight;
bool		flooron = true;
bool		g_block = false;

////  CLOTH application globals (start sizes)
cloth *		g_cloth;					/// this is the global cloth class instance
int			g_clothlength = 8;			/// This is actually how many verticies are down each side (not its physical length)
int			g_clothheight = 8;			/// 8 is a good value
int			g_mass = 3;					/// mass weight for unconstrained verticies
float		g_XSize = XNATLENGTH;		/// length between each cloth vertex in world coordinates to start
float		g_YSize = YNATLENGTH;
bool		g_drawball = false;			/// whether or not to draw the arcball
// const int	matrixsize = 27;			/// global to tell us how big the model view matrix is
// #define matrixsize 27; // Can't use extern const with array initializations
frame *		cur_frame;

// OpenGL non-shader handles
GLuint		h_texture;					/// handle to texture

// shader globals
GLuint		h_program;					/// handle to OpenGL program object
GLint		h_uProjMatrix;				/// handle to unifrom var for projection matrix
GLint		h_uModelViewMatrix;			/// handle to uniform var for model view matrices (array)
GLint		h_uNormalMatrix;	
GLuint		h_vTexCoord;				/// handle to attribute var for texture coordinates
GLuint		h_texUnit0;					/// handle to texture unit variable in fragment program
GLint		h_uLight;					/// handle to uniform variable for light vector
GLint		h_vColor;					/// handle to attribute variable for color of vertex
GLint		h_drawcloth;				/// handle to int u_drawcloth in vertex shader
GLint		h_vMVIndex;					/// hangle to vMVIndex in vertex shader

bool		fullscreen = true;			// Fullscreen Flag Set To Fullscreen Mode By Default
