#include "stdafx.h"
#include "glFuncs.h"
#include "GLOBALS.h"
#include "timeKeeper.h"

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
	glUniform1i(h_vMVIndex, 0);	
	glUniform1i(h_drawcloth, 1);
	g_cloth->DrawCloth(h_vTexCoord,g_block);

	// turn off cloth drawing
	glUniform1i(h_drawcloth, 0);

	if(flooron) {
		// draw the floor
		glUniform1i(h_vMVIndex, 0);
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
	glUniform1i(h_vMVIndex, 0);
	glVertexAttrib3f(h_vColor, 0.5, 0.5, 0.5);
	if(g_drawball) {
		glutWireSphere(1.8, 25, 25);
	}

	if(DRAWSPHERE) {
		glUniform1i(h_vMVIndex, 1);
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
	case ('n'): {
		while(cur_frame->lastframe != NULL) { // Go to the start of the list
			cur_frame = cur_frame->lastframe;
		}
		cur_frame->UpdateToFrame(*g_cloth);
		glutPostRedisplay();
		display();
				break;
    }
	case ('r'): {
		while(cur_frame->lastframe != NULL) { // Go to the start of the list
			cur_frame = cur_frame->lastframe;
		}
		printf("\nAt start\n");
		cur_frame->UpdateToFrame(*g_cloth);
		glutPostRedisplay();
		display();

		timeKeeper * time = new timeKeeper();
		float curTime = time->GetTime();
		float lastTime = curTime;
		float accumulator = 0.0f;

		// do an arc ball routine to make it look nice
		rbt new_O_frame;
		rbt O_frame;
		rbt S_frame;
		rbt inv_S_frame;

		// find the qrot (from class notes it is [0, v1].[0, v0] hence w = 0) and therefore the rbt Q
		// This is to spin the ball around
		qrot rot;
		rbt Q;

		float T_FPS = 1.0 / 60.0;

		// Axis angle to quaternion
		//http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
		coords3 axis;
		axis.x = 0.0; axis.y = 1.0; axis.z = 0.0;
		float angle = T_FPS * (45.0/360.0)*(2.0*3.141596); // (rad / sec) * (sec / frame) = rad / frame
		double mult = sin(angle / 2.0);

		rot.x = axis.x * mult;
		rot.y = axis.y * mult;
		rot.z = axis.z * mult;
		rot.w = cos(angle / 2.0);
		// normalize the rotation
		double mag = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w);
		rot.x = rot.x / mag;
		rot.y = rot.y / mag;
		rot.z = rot.z / mag;
		rot.w = rot.w / mag;
		Q = rbt(rot);

		while(cur_frame->nextframe != NULL) {

			// Spin wait while we get to the next timeStep
			while(accumulator <= T_FPS)
			{	
				curTime = time->GetTime();
				accumulator += curTime - lastTime;
				lastTime = curTime;
			}
			// Step to the next frame
			cur_frame = cur_frame->nextframe;

			// Remove an HSTEP from the accumulator since we've eaten it up
			accumulator -= T_FPS;

			cur_frame = cur_frame->nextframe;
			cur_frame->UpdateToFrame(*g_cloth);

			S_frame.translation = g_base_frame.translation;
			S_frame.rotation = g_sky_camera.rotation;
			O_frame = g_sky_camera;
			inv_S_frame = S_frame.GetInverse();	

			// CARRY OUT THE O' = SQS^(-1)O ROUTINE
			new_O_frame = S_frame * Q * inv_S_frame * O_frame;
			g_sky_camera = new_O_frame;

			glutPostRedisplay();
			display();
		}

		delete time;

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

		float time =  0.5;

		float FPS = 60;
		float t_FPS = 1.0/60.0;
		int frameSkip = (int)((t_FPS / HSTEP) + 0.00001f); // Don't save every frame to the linked-list --> Waste of memory
		if(abs(frameSkip - t_FPS / HSTEP) > 0.0001f)
			throw std::exception("t_FPS is not an integer multiple of HSTEP");
		int numberframes = (int)(time / t_FPS + 0.00001f);

		// go to the last frame
		while(cur_frame->nextframe != NULL) {
			cur_frame = cur_frame->nextframe;
		}
		cur_frame->UpdateToFrame(*g_cloth);

		for (int i = 0; i<numberframes; i++) {

			for(int j = 0; j < frameSkip; j ++)
				TakeStep();

			// get the next frame and add it to the end of the list of frames
			frame * temp;
			temp = new frame((g_cloth->numberverticies));
			temp->SetFrame(*(g_cloth->POS));
			cur_frame->nextframe = temp;
			temp->lastframe = cur_frame;
			cur_frame = temp;
			printf("made frame %fsec of %fsec\n",((float)(i+1))*t_FPS,((float)numberframes)*t_FPS);
			display();
		}
		printf("%fsec of frames made sucessfully!!\n",((float)numberframes)*t_FPS);
		}
		break;

	case ('m'): {
		while(cur_frame->lastframe != NULL) { // Go to the start of the list
			cur_frame = cur_frame->lastframe;
		}
		printf("\nAt start\n");
		cur_frame->UpdateToFrame(*g_cloth);

		BeginMovie(g_width, g_height, KSTRETCH, KDSTRETCH, KSHEAR, KDSHEAR, KBEND, 
			KDBEND, HSTEP);
		unsigned char *screendat = new unsigned char[g_width * g_width*100];	

		glutPostRedisplay();
		display();
		glReadPixels(0, 0, g_width, g_height, GL_BGRA, GL_UNSIGNED_BYTE, screendat);
		UpdateMovie(g_width, g_height, screendat);

		// do an arc ball routine to make it look nice
		rbt new_O_frame;
		rbt O_frame;
		rbt S_frame;
		rbt inv_S_frame;

		// find the qrot (from class notes it is [0, v1].[0, v0] hence w = 0) and therefore the rbt Q
		// This is to spin the ball around
		qrot rot;
		rbt Q;

		float T_FPS = 1.0 / 60.0; // trial an error to find movie FPS

		// Axis angle to quaternion
		//http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
		coords3 axis;
		axis.x = 0.0; axis.y = 1.0; axis.z = 0.0;
		float angle = T_FPS * (45.0/360.0)*(2.0*3.141596); // (rad / sec) * (sec / frame) = rad / frame
		double mult = sin(angle / 2.0);

		rot.x = axis.x * mult;
		rot.y = axis.y * mult;
		rot.z = axis.z * mult;
		rot.w = cos(angle / 2.0);
		// normalize the rotation
		double mag = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w);
		rot.x = rot.x / mag;
		rot.y = rot.y / mag;
		rot.z = rot.z / mag;
		rot.w = rot.w / mag;
		Q = rbt(rot);

		while(cur_frame->nextframe != NULL) {
			cur_frame = cur_frame->nextframe;
			cur_frame->UpdateToFrame(*g_cloth);

			S_frame.translation = g_base_frame.translation;
			S_frame.rotation = g_sky_camera.rotation;
			O_frame = g_sky_camera;
			inv_S_frame = S_frame.GetInverse();	

			// CARRY OUT THE O' = SQS^(-1)O ROUTINE
			new_O_frame = S_frame * Q * inv_S_frame * O_frame;
			g_sky_camera = new_O_frame;

			glutPostRedisplay();
			display();
			glReadPixels(0, 0, g_width, g_height, GL_BGRA, GL_UNSIGNED_BYTE, screendat);
			UpdateMovie(g_width, g_height, screendat);
		}

		EndMovie();
		delete [] screendat;

		printf("movie finished \n");
		printf("\nAt end\n");

		/*
		float fps = 60.0; // This is set by the codec usually --> Just trial and error it
		float t_movie = 1.0 / fps;

		float movieTime = 3;
		int numberframes = (int)(movieTime / t_movie);

		BeginMovie(g_width, g_height, KSTRETCH, KDSTRETCH, KSHEAR, KDSHEAR, KBEND, 
			KDBEND, HSTEP);
		unsigned char *screendat = new unsigned char[g_width * g_width*100];	

		// do an arc ball routine to make it look nice
		rbt new_O_frame;
		rbt O_frame;
		rbt S_frame;
		rbt inv_S_frame;

		// find the qrot (from class notes it is [0, v1].[0, v0] hence w = 0) and therefore the rbt Q
		// This is to spin the ball around
		qrot rot;
		rbt Q;

		// Axis angle to quaternion
		//http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
		coords3 axis;
		axis.x = 0.0; axis.y = 1.0; axis.z = 0.0;
		float angle = HSTEP * (10.0/360.0)*(2.0*3.141596);
		double mult = sin(angle / 2.0);

		rot.x = axis.x * mult;
		rot.y = axis.y * mult;
		rot.z = axis.z * mult;
		rot.w = cos(angle / 2.0);
		// normalize the rotation
		double mag = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w);
		rot.x = rot.x / mag;
		rot.y = rot.y / mag;
		rot.z = rot.z / mag;
		rot.w = rot.w / mag;
		Q = rbt(rot);

		float accumulator = 0.0f;
		float movie_FPS = 60.0f; // Match the FPS of the movie codec.  This is a magic number...  Make the movie, look at the FPS and adjust this.
		float movie_Tstep = 1.0f / movie_FPS;

		for (int i = 0; i < numberframes; i++)
		{	
			glReadPixels(0, 0, g_width, g_height, GL_BGRA, GL_UNSIGNED_BYTE, screendat);
			UpdateMovie(g_width, g_height, screendat);

			while((accumulator - movie_Tstep) < -EPSILON)
			{
				TakeStep();
				accumulator += HSTEP;

				S_frame.translation = g_base_frame.translation;
				S_frame.rotation = g_sky_camera.rotation;
				O_frame = g_sky_camera;
				inv_S_frame = S_frame.GetInverse();	

				// CARRY OUT THE O' = SQS^(-1)O ROUTINE
				new_O_frame = S_frame * Q * inv_S_frame * O_frame;
				g_sky_camera = new_O_frame;

				glutPostRedisplay();
				display();  
				
			}

			accumulator = 0.0f;
			printf("made frame %d of %d\n",i+1,numberframes);
		}
		EndMovie();
		printf("movie finished \n");
		glutPostRedisplay();	
		delete [] screendat;
				}
				*/
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
	pixel *pixdata = ppmread("hipster_resized.ppm", &twidth, &theight); // read in the texture (sqwirrle pick)
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
	//if ((h_drawcloth = glGetAttribLocation(h_program, "v_drawcloth"))<0)
	//{ fprintf(stderr, "Error getting v_drawcloth variable\n"); return 1; }
	//if ((h_vMVIndex = glGetAttribLocation(h_program, "vMVIndex"))<0)
	//{ fprintf(stderr, "Error getting v_drawcloth variable\n"); return 1; }

	// CHANGE - OCTOBER 2010
	if ((h_drawcloth = glGetUniformLocation(h_program, "uDrawCloth"))<0)
	{ fprintf(stderr, "Error getting udrawCloth variable\n"); return 1; }
	if ((h_vMVIndex = glGetUniformLocation(h_program, "vMVIndex"))<0)
	{ fprintf(stderr, "Error getting v_drawcloth variable\n"); return 1; }

	return 0;
}