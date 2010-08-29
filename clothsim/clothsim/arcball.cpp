/////////////////////////////////////////////////////////////////////////////
//
//	arcball.cpp
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

#include "StdAfx.h"
#include "arcball.h"


// constants
const float	FRUST_FOVY = 60.0;		/// 60 degree field of view in y direction
const float	FRUST_NEAR = -0.1;		/// near plane
const float FRUST_FAR  = -50.0;		/// far plane
const float bodysize = 0.6;
const float arcballradius = 3*bodysize;

/*  EMAIL FROM HAMILTON CHONG
Ok, helper code for assignment 3's arcball has been posted on the course
web site (under Code From Class subheading).  This code takes a sphere you
specify (along with projection matrix and viewport information) and spits
back out a circle in screen space (pixel coordinates and radius in pixels)
corresponding to it.

Paste the function into your asst3.cpp file under the Helper Functions
section.  Example usage is also included:

  // You use the function something like:
  // test out screen space circle
  matrix4 projmat = MakeProjection(FRUST_FOVY, (float)g_width/(float)g_height, FRUST_NEAR, FRUST_FAR);
  coords3 screencenter;
  double screenrad;
 
  GetScreenSpaceCircle(coords3(0.0, 0.0, 1.7), 0.5, projmat, g_width, g_height, screencenter, screenrad);
  printf("center = (%f, %f, %f), radius = %f\n", screencenter.x, screencenter.y, screencenter.z, screenrad);
  GetScreenSpaceCircle(coords3(1.0, 0.0, -1), tan(FRUST_FOVY*PI/360.0), projmat, g_width, g_height, screencenter, screenrad);
  printf("center = (%f, %f, %f), radius = %f\n", screencenter.x, screencenter.y, screencenter.z, screenrad);
*/

void GetScreenSpaceCircle(const coords3& center, double radius,		// camera/eye coordinate info for sphere
						  const matrix4& projection,				// projection matrix
						  unsigned int screen_width, unsigned int screen_height, // viewport size
						  coords3& out_center, double& out_radius)	// output data in screen coordinates
{
	// get post projection canonical coordinates
	coords3 postproj = projection * center;
	double w = projection.elt[3][0] * center.x + projection.elt[3][1] * center.y + projection.elt[3][2] * center.z + projection.elt[3][3] * 1.0;
	double winv = 0.0;
	if (w != 0.0)
		winv = 1.0 / w;
	postproj *= winv;

	// convert to screen pixel space
	out_center = postproj;
	out_center.x = out_center.x * (double)screen_width/2.0 + ((double)screen_width-1.0)/2.0;
	out_center.y = out_center.y * (double)screen_height/2.0 + ((double)screen_height-1.0)/2.0;

	// determine some overall radius
	double dist = center.z;
	if (dist < FRUST_NEAR)
		out_radius = -radius/(dist * tan(FRUST_FOVY*PI/360.0));
	else
		out_radius = 0.0;

	out_radius *= screen_height*0.5;

}

// arcball function to do the arcball routine in mouse move
void arcball (rbt &O_frame_in_eye, rbt &O_frame, rbt &new_O_frame, rbt &S_frame, rbt &inv_S_frame, 
			  int g_width, int g_height, double old_x, double old_y, double new_x, 
			  double new_y, int g_manip_object) {
	// temp variables
	double dx, dy, dz, mag;
	bool out_sphere;
	coords3 v1, v2, a;
	qrot rot, q1, q2;
	rbt Q;

	// first get the center of the arcball
	coords3 center = O_frame_in_eye.translation;

	// now set up the projection matrix and variables to hold GetScreenSpaceCircle
	matrix4 projmat = MakeProjection(FRUST_FOVY, (float)g_width/(float)g_height, FRUST_NEAR, FRUST_FAR);
	coords3 screencenter;
	double screenrad;
	//get the center and radius in pixles
	GetScreenSpaceCircle(center, arcballradius, projmat, g_width, g_height, screencenter, screenrad);
	
	// VECTOR 1 - OLD POINT
	dx = old_x - screencenter.x;
	dy = old_y - screencenter.y;
	mag = (double)sqrt(dx*dx + dy*dy);
	out_sphere = (mag > screenrad);
	// if the point is out of the sphere, clamp to edge
	if(out_sphere == true) {
		//normalize dx and dy to clamp dz to 0
		dx = (dx / mag);
		dy = (dy / mag);
		dz = 0.0;
	} else {
		//solve for z (in X^2 + Y^2 + Z^2 = r^2)
		dz = (double)sqrt(screenrad*screenrad - (dx*dx) - (dy*dy));
		//normalize
		mag = (double)sqrt(dx*dx + dy*dy + dz*dz); 
		dx = dx / mag;
		dy = dy / mag;
		dz = dz / mag;
	}
	v1.SetCoords(dx,dy,dz);		//and we have our vector

	// VECTOR 2 - NEW POINT
	dx = (double)new_x - screencenter.x;
	dy = (double)new_y - screencenter.y;
	mag = (double)sqrt(dx*dx + dy*dy);
	out_sphere = (mag > screenrad);
	// if the point is out of the sphere, clamp to edge
	if(out_sphere == true) {
		//normalize dx and dy to clamp dz to 0
		dx = dx / mag;
		dy = dy / mag;
		dz = 0.0;
	} else {
		//solve for z (in X^2 + Y^2 + Z^2 = r^2)
		dz = (double)sqrt(screenrad*screenrad - (dx*dx) - (dy*dy));
		//normalize
		mag = (double)sqrt(dx*dx + dy*dy + dz*dz); 
		dx = dx / mag;
		dy = dy / mag;
		dz = dz / mag;
	}
	v2.SetCoords(dx,dy,dz);		//and we have our vector

	// find the qrot (from class notes it is [0, v1].[0, v0] hence w = 0) and therefore the rbt Q
	q1 = qrot(v1.x, v1.y, v1.z, 0.0);
	q2 = qrot(v2.x, v2.y, v2.z, 0.0);
	
	if (g_manip_object == 2)
		rot = q1*q2;
	else 
		rot = q2 * q1;	
	Q = rbt(rot);

	// CARRY OUT THE O' = SQS^(-1)O ROUTINE
	new_O_frame = S_frame * Q * inv_S_frame * O_frame;
}