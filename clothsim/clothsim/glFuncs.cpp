#include "stdafx.h"
#include "glFuncs.h"
#include "GLOBALS.h"

const int matrixsize = 27; // Can't use extern const with array initializations

// C A L L B A C K S ///////////////////////////////////////////////////

// _____________________________________________________
//|														|
//|	 display											|
//|_____________________________________________________|
///
///  Whenever OpenGL requires a screen refresh
///  it will call display() to draw the scene.
///  We specify that this is the correct function
///  to call with the glutDisplayFunc() function
///  during initialization



void  setMatrices(void)
{
	g_eye_matrix = g_sky_camera;

	// set camera's projection matrix
	matrix4 projmat;
	GLfloat glprojmat[16];
	projmat = MakeProjection(FRUST_FOVY, (float)g_width/(float)g_height, FRUST_NEAR, FRUST_FAR);
	projmat.Mat2GL(glprojmat);
	glUniformMatrix4fv(h_uProjMatrix, 1, GL_FALSE, glprojmat);

	// set camera's modelview matrix
	rbt inv_eye = g_eye_matrix.GetInverse();
	GLfloat matrixdata[(16 * matrixsize)];
	GLfloat normaldata[(16 * matrixsize)];
	int j = 0;										// keeps track of current index

	// Floor and cloth
	rbt modelview = g_base_frame * inv_eye;
	modelview.Rbt2GL(&matrixdata[j*16]);
	matrix4 normalmat = (modelview.GetMatrix()).GetNormalMatrix();
	normalmat.Mat2GL(&normaldata[j*16]);

	// The blocking sphere
	j++;
	modelview = g_base_frame * inv_eye * g_cloth->spherecenter;
	modelview.Rbt2GL(&matrixdata[j*16]);
	normalmat = (modelview.GetMatrix()).GetNormalMatrix();
	normalmat.Mat2GL(&normaldata[j*16]);

	// load the light position in eye space
	glUniform3f(h_uLight, 0.0, 0.0, 5.0);

	// send off the matrix data into the vertex shader variables: uModelViewMatrix & uNormalMatrix
	glUniformMatrix4fvARB(h_uModelViewMatrix, matrixsize, GL_FALSE, matrixdata);
	glUniformMatrix4fvARB(h_uNormalMatrix,  matrixsize, GL_FALSE, normaldata);
}


void drawStuff(void)
{
	// turn on cloth drawing and draw the cloth
	glVertexAttrib1f(h_vMVIndex, 0.0);	
	glVertexAttrib1f(h_drawcloth, 1.0);
	g_cloth->DrawCloth(h_vTexCoord,g_block);

	// turn off cloth drawing
	glVertexAttrib1f(h_drawcloth, 0.0);

	if(flooron) {
		// draw the floor
		glVertexAttrib1f(h_vMVIndex, 0.0);
		glNormal3f(0.0, 1.0, 0.0);
		glBegin(GL_TRIANGLES);
		glVertexAttrib3f(h_vColor, 0.3, 0.7, 0.8);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f( FLOOR_SIZE, FLOOR_Y,  FLOOR_SIZE);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f( FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-FLOOR_SIZE, FLOOR_Y,  FLOOR_SIZE);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f( FLOOR_SIZE, FLOOR_Y,  FLOOR_SIZE);
		glEnd();
	}

	//draw arcball in center of world if we need it
	glVertexAttrib1f(h_vMVIndex, 0.0);
	glVertexAttrib3f(h_vColor, 0.5, 0.5, 0.5);
	if(g_drawball) {
		glutWireSphere(1.8, 25, 25);
	}

	if(DRAWSPHERE) {
		glVertexAttrib1f(h_vMVIndex, 1.0);
		glNormal3f(0.0, 1.0, 0.0);
		glutWireSphere(SPHERERADIUS-0.03, 25, 25);
	}
}


void display(void)
{ 
	glUseProgram(h_program);

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the variables to the shaders
	setMatrices();

	// issue the draw calls
	drawStuff();

	// display the back buffer
	glutSwapBuffers();

	// check for errors
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR){
		const GLubyte *errString;
		errString=gluErrorString(errCode);
		printf("error: %s\n", errString);
	}

}


// _____________________________________________________
//|														|
//|	 reshape											|
//|_____________________________________________________|
///
///  Whenever a window is resized, a "resize" event is
///  generated and glut is told to call this reshape
///  callback function to handle it appropriately.

void reshape(int w, int h)
{
	g_width = w;
	g_height = h;
	glViewport(0, 0, w, h);
	//printf("Size of window is now  %d x %d\n", w, h);
	glutPostRedisplay();
}


// _____________________________________________________
//|														|
//|	 mouse												|
//|_____________________________________________________|
///
///  Whenever a mouse button is clicked, a "mouse" event
///  is generated and this mouse callback function is
///  called to handle the user input.

void mouse(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) 
		{           
			// left mouse button has been clicked
			g_left_clicked = true;
			g_lclick_x = x;
			g_lclick_y = g_height - y - 1;
			g_drawball = true;
			glutPostRedisplay();
		}  else {
			// left mouse button has been released
			g_left_clicked = false;
			g_drawball = false;
			glutPostRedisplay();
		}
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN) {           
			// right mouse button has been clicked
			g_right_clicked = true;
			g_rclick_x = x;
			g_rclick_y = g_height - y - 1;
			glutPostRedisplay();
		} else {
			// right mouse button has been released
			g_right_clicked = false;
			glutPostRedisplay();
		}
	}
}


// _____________________________________________________
//|														|
//|	 mousemove											|
//|_____________________________________________________|
///
///  Whenever the mouse is moved while a button is pressed, 
///  a "mouse move" event is triggered and this callback is 
///  called to handle the event.

static void mousemove(int x, int y)
{	
	//printf("%d, %d\n",x,y);
	// temp values for frame values
	rbt new_O_frame;
	rbt O_frame;
	rbt S_frame;
	rbt inv_S_frame;
	rbt body;
	// temp values for other stuff
	rbt trans;
	double scale = 0.025;	// this is the scale to translate

	// get newx information
	int newx = x;
	int newy = g_height - y - 1;

	S_frame.translation = g_base_frame.translation;
	S_frame.rotation = g_sky_camera.rotation;
	O_frame = g_sky_camera;
	inv_S_frame = S_frame.GetInverse();	

	if (g_left_clicked && !g_right_clicked) {
		// work out what the O_frame is w.r.t eye coords (needed for GetScreenSpaceCircle)
		rbt inv_eye = g_eye_matrix.GetInverse();
		rbt modelview = g_base_frame * inv_eye;
		// sent everything off to arcball function
		arcball(modelview, O_frame, new_O_frame, S_frame, inv_S_frame, 
			g_width, g_height, g_lclick_x, g_lclick_y, newx, newy, 2);

		// set the g_lclick values
		g_lclick_x = newx;
		g_lclick_y = newy;
	}

	if (g_right_clicked && !g_left_clicked) {
		// get deltax and deltay information
		float deltax = newx - g_rclick_x;
		float deltay = newy - g_rclick_y;
		// CARRY OUT THE O' = SQS^(-1)O ROUTINE
		trans.rotation = QrotIdent();
		trans.translation.SetCoords(scale * deltax, scale * deltay, 0.0);
		new_O_frame = S_frame * trans * inv_S_frame * O_frame;  // find the new object frame
		// set the g_rclick values
		g_rclick_x = newx;
		g_rclick_y = newy;
	}

	if (g_right_clicked && g_left_clicked) {
		// get the deltay info, set up the trans matrix and perform it
		float deltay = newy - g_rclick_y;
		trans.rotation = QrotIdent();
		trans.translation.SetCoords(0.0, 0.0, -scale * deltay);
		new_O_frame = S_frame * trans * inv_S_frame * O_frame;  // find the new object frame

		// set the g_rclick values
		g_rclick_x = newx;
		g_rclick_y = newy;
		g_lclick_y = newy;
		g_lclick_x = newx;
	}

	// translate the skycamera
	g_sky_camera = new_O_frame;

	// redraw the scene
	glutPostRedisplay();
}

// this function removes stuff from memory and quits.  Self explanitory really.
void quit (void) {
	/* destroy the OpenGL window */
	glutDestroyWindow (glutGetWindow());
	printf("\n\n\t\tBYE BYE!!\n\n");

	exit (1);
}

void KeyFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'h':
		printf(" ============== H E L P ==============\n\n");
		printf("PROGRAM FUNCTIONS:\n");
		printf("h\t\thelp menu\n");
		printf("q\t\tquit the program\n");
		printf("\nMOVEMENT:\n");
		printf("\t\tdrag left mouse to rotate\n");
		printf("\t\tdrag right mouse to translate\n");
		printf("\t\tdrag double click to move in and out\n");
		printf("\nOTHER USER INTERFACE:\n");
		printf("s\t\tsave screenshot\n");
		printf("f\t\tturn the floor on and off\n");
		printf("b\t\tturn block shading on and off\n");
		printf("t\t\titerate forward one frame and add it to frame list\n");
		printf("a\t\titerate forward 150 frames and add them to the frame list\n");
		printf("<\t\tmove back a frame in frame list\n");
		printf(">\t\tmove forward a frame in frame list\n");
		printf("r\t\trun through all the frames from start to finish\n");
		printf("m\t\tmake a movie from current conditions\n\n");
		printf("\n");
		printf("NOTE: depending upon the video coder used, the quality will vary\n");
		break;
	case 's':
		{
			glFlush();
			pixel *imFromScreen;
			int size = g_width * g_height * sizeof(pixel);
			imFromScreen = (pixel *)malloc(size);
			glReadPixels(0,0,g_width,g_height, GL_RGB, GL_UNSIGNED_BYTE, imFromScreen);
			int ret = ppmwrite("out.ppm",g_width, g_height,  imFromScreen);
			if (ret != -1)
				printf("out.ppm written\n");
			else
				printf("failed to write out.ppm\n");
		}
		break;
	case ('q'): {
		quit();
				}
				break;
	case ('t'): {
		// go to the last frame
		printf("please wait, taking a step\n");
		while(cur_frame->nextframe != NULL) {
			cur_frame = cur_frame->nextframe;
		}
		cur_frame->UpdateToFrame(*g_cloth);
		TakeStep();

		glutPostRedisplay();
		// get the next frame and add it to the end of the list of frames
		frame * temp;
		temp = new frame((g_cloth->numberverticies));
		temp->SetFrame(*(g_cloth->POS));
		cur_frame->nextframe = temp;
		temp->lastframe = cur_frame;
		cur_frame = temp;
		printf("STEP TAKEN SUCCESSFULLY!\n");
				}
				break;
	case ('f'): {
		if(flooron==true)
			flooron=false;
		else
			flooron=true;
		glutPostRedisplay();
				}
				break;
	case ('b'): {
		if(g_block==true){
			g_block=false;
			printf("block shading off\n"); }
		else {
			g_block=true;
			printf("block shading on\n");
		} display();
				}
				break;
	case ('r'): {
		while(cur_frame->lastframe != NULL) {
			cur_frame = cur_frame->lastframe;
		}
		printf("\nAt start\n");
		cur_frame->UpdateToFrame(*g_cloth);
		glutPostRedisplay();
		display();

		// do an arc ball routine to make it look nice
		rbt new_O_frame;
		rbt O_frame;
		rbt S_frame;
		rbt inv_S_frame;

		while(cur_frame->nextframe != NULL) {
			cur_frame = cur_frame->nextframe;
			cur_frame->UpdateToFrame(*g_cloth);

			/*S_frame.translation = g_base_frame.translation;
			S_frame.rotation = g_sky_camera.rotation;
			O_frame = g_sky_camera;
			inv_S_frame = S_frame.GetInverse();	

			// work out what the O_frame is w.r.t eye coords (needed for GetScreenSpaceCircle)
			rbt inv_eye = g_eye_matrix.GetInverse();
			rbt modelview = g_base_frame * inv_eye;
			// sent everything off to arcball function
			arcball(modelview, O_frame, new_O_frame, S_frame, inv_S_frame, 
			g_width, g_height, 900, 600, 899.6, 600, 2);
			// translate the skycamera
			g_sky_camera = new_O_frame;*/

			glutPostRedisplay();
			display();
		}
		printf("\nAt end\n");
				}
				break;
	case ('<'): {
		if(cur_frame->lastframe == NULL)
			printf("Already on first frame\n");
		else {
			cur_frame = cur_frame->lastframe;
			cur_frame->UpdateToFrame(*g_cloth);
			glutPostRedisplay();
		}
				}
				break;
	case ('>'): {
		if(cur_frame->nextframe == NULL)
			printf("Already on last frame\n");
		else {
			cur_frame = cur_frame->nextframe;
			cur_frame->UpdateToFrame(*g_cloth);
			glutPostRedisplay();
		}
				}
				break;
	case ('a'): {
		int numberframes = 150;
		double numberminutes = ((double)numberframes*(1.5 / 60.0));
		printf("please wait, drawing frames...\navg drawing time at 1.5 sec per frame:");
		printf("%d minutes, %f seconds\n",(int)numberminutes, (numberminutes - (int)numberminutes)*60);
		// go to the last frame
		while(cur_frame->nextframe != NULL) {
			cur_frame = cur_frame->nextframe;
		}
		cur_frame->UpdateToFrame(*g_cloth);

		for (int i = 0; i<numberframes; i++) {
			TakeStep();
			// go to the end of the frames
			while(cur_frame->nextframe != NULL) {
				cur_frame = cur_frame->nextframe;
			}

			// get the next frame and add it to the end of the list of frames
			frame * temp;
			temp = new frame((g_cloth->numberverticies));
			temp->SetFrame(*(g_cloth->POS));
			cur_frame->nextframe = temp;
			temp->lastframe = cur_frame;
			cur_frame = temp;
			printf("made frame %d of %d\n",i+1,numberframes);
			display();
		}
		printf("%d frames made sucessfully!!\n",numberframes);
				}
				break;

	case ('m'): {
		int numberframes = 750;
		double numberminutes = ((double)numberframes*(1.5 / 60.0));
		printf("please wait, making a movie...\navg drawing time at 1 sec per frame:");
		printf("%d minutes, %f seconds\n",(int)numberminutes, (numberminutes - (int)numberminutes)*60);
		BeginMovie(g_width, g_height, KSTRETCH, KDSTRETCH, KSHEAR, KDSHEAR, KBEND, 
			KDBEND, HSTEP);

		unsigned char *screendat = new unsigned char[g_width * g_width*100];	

		// do an arc ball routine to make it look nice
		rbt new_O_frame;
		rbt O_frame;
		rbt S_frame;
		rbt inv_S_frame;

		for (int i = 0; i < numberframes; i++)
		{		
			glReadPixels(0, 0, g_width, g_height, GL_BGRA, GL_UNSIGNED_BYTE, screendat);
			UpdateMovie(g_width, g_height, screendat);
			TakeStep();

			S_frame.translation = g_base_frame.translation;
			S_frame.rotation = g_sky_camera.rotation;
			O_frame = g_sky_camera;
			inv_S_frame = S_frame.GetInverse();	

			// work out what the O_frame is w.r.t eye coords (needed for GetScreenSpaceCircle)
			rbt inv_eye = g_eye_matrix.GetInverse();
			rbt modelview = g_base_frame * inv_eye;
			// sent everything off to arcball function
			arcball(modelview, O_frame, new_O_frame, S_frame, inv_S_frame, 
				g_width, g_height, 512, 450, 511.7, 450, 2);
			// translate the skycamera
			g_sky_camera = new_O_frame;


			display();  
			printf("made frame %d of %d\n",i+1,numberframes);
		}
		EndMovie();
		printf("movie finished \n");
		glutPostRedisplay();	
		delete [] screendat;
				}
				break;		
	}
}

// H E L P E R    F U N C T I O N S ////////////////////////////////////

void initGlutState(int argc, char **argv) {
	glutInit(&argc,argv);					// initialize Glut based on cmd-line args
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);	//  RGBA pixel channels and double buffering

	glutInitWindowSize(g_width, g_height);	// create a window
	glutCreateWindow("Jonno and Andre's fabulous project");	// title the window

	glutDisplayFunc(display);					// display rendering callback
	glutReshapeFunc(reshape);					// window reshape callback
	glutMotionFunc(mousemove);				// mouse movement callback
	glutMouseFunc(mouse);						// mouse click callback
	glutKeyboardFunc(KeyFunc);
}

void InitGLState(void)
{
	glClearColor(0.,0.2 ,0.,0.);
	glClearDepth(0.0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);			// ******* NOTE: CHANGED!!!!!   ***************

	// set up texture
	glActiveTexture(GL_TEXTURE0);
	int twidth, theight;
	pixel *pixdata = ppmread("reachup.ppm", &twidth, &theight); // read in the texture (sqwirrle pick)
	g_twidth = twidth;
	g_theight = theight;
	glGenTextures(1, &h_texture);
	glBindTexture(GL_TEXTURE_2D, h_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixdata);
}

int InitGLHandles(void) {
	// grab handles to the shader variables by name
	if ((h_uProjMatrix = glGetUniformLocation(h_program, "uProjMatrix"))<0)
	{ fprintf(stderr, "Error getting uProjMatrix variable\n"); return 1; }
	if ((h_uModelViewMatrix = glGetUniformLocation(h_program, "uModelViewMatrix"))<0)
	{ fprintf(stderr, "Error getting uModelViewMatrix variable\n"); return 1; }
	if ((h_uNormalMatrix = glGetUniformLocation(h_program, "uNormalMatrix"))<0)
	{ fprintf(stderr, "Error getting uNormalMatrix variable\n"); return 1; }
	if ((h_vTexCoord = glGetAttribLocation(h_program, "vTexCoord"))<0)
	{ fprintf(stderr, "Error getting vTexCoord variable\n"); return 1; }
	if ((h_texUnit0 = glGetAttribLocation(h_program, "texUnit0"))<0)
	{ fprintf(stderr, "Error getting texUnit0 variable\n"); return 1; }
	if ((h_uLight = glGetUniformLocation(h_program, "uLight"))<0)
	{ fprintf(stderr, "Error getting uLight variable\n"); return 1; }
	if ((h_vColor = glGetAttribLocation(h_program, "vColor"))<0)
	{ fprintf(stderr, "Error getting vColor variable\n"); return 1; }
	if ((h_drawcloth = glGetAttribLocation(h_program, "v_drawcloth"))<0)
	{ fprintf(stderr, "Error getting v_drawcloth variable\n"); return 1; }
	if ((h_vMVIndex = glGetAttribLocation(h_program, "vMVIndex"))<0)
	{ fprintf(stderr, "Error getting v_drawcloth variable\n"); return 1; }
	return 0;
}