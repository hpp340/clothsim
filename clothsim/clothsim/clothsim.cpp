////////////////////////////////////////////////////////////////////////////////
//																			  //
//   CS175 : Computer Graphics												  //
//																			  //
//   File: fp.cpp															  //
//   Name: Jonathan T, Andre M												  //
//   Date: 1/1/2006															  //
//   Desc: This is our final project, note: skelleton code taken from asst5   //
//         and adapted for this project. some code taken from Jonathan's 	  //
//		   Asst3															  //
//																			  //
////////////////////////////////////////////////////////////////////////////////
//
// OPENMP SUPPORT MUST BE TURNED ON IN THE PROPERTY PAGES, THOUGH IT SHOULD BE.
// SEE "Jonno.Setting up glew and glut.txt" FOR LIBRARY AND DLL DETAILS.
//
////////////////////////////////////////////////////////////////////////////////

// I N C L U D E S    &    G L O B A L S ///////////////////////////////////////
#include "stdafx.h"
#include "GLOBALS.h"

using namespace std;

// H E L P E R    F U C T I O N S ///////////////////////

void setupState(void) {
	// now set up initial system state
	g_base_frame.rotation = QrotIdent();
	g_base_frame.translation.SetCoords(0.0, 0.0, 0.0);

	g_sky_camera.rotation = MakeXQrot(0.0);
	g_sky_camera.translation.SetCoords(0.0, 0.0, 5 );
	g_eye_matrix = g_sky_camera;

	// do an arc ball routine to make it look nice
	rbt new_O_frame;
	rbt O_frame;
	rbt S_frame;
	rbt inv_S_frame;

	S_frame.translation = g_base_frame.translation;
	S_frame.rotation = g_sky_camera.rotation;
	O_frame = g_sky_camera;
	inv_S_frame = S_frame.GetInverse();	

	// work out what the O_frame is w.r.t eye coords (needed for GetScreenSpaceCircle)
	rbt inv_eye = g_eye_matrix.GetInverse();
	rbt modelview = g_base_frame * inv_eye;
	// sent everything off to arcball function
	arcball(modelview, O_frame, new_O_frame, S_frame, inv_S_frame, 
		g_width, g_height, 832, 577, 748, 500, 2);
	// translate the skycamera
	g_sky_camera = new_O_frame;


	//Set up each of the cloths, and initialize them
	g_cloth = new cloth(g_clothlength, g_clothheight);
	g_cloth->InitCloth(g_XSize, g_YSize, g_mass);

	// Set up the blocking sphere stuff
	g_cloth->spherecenter.rotation = QrotIdent();
	g_cloth->spherecenter.translation.SetCoords(SPHERE_X, SPHERE_Y, SPHERE_Z);
	g_cloth->sphereradius = SPHERERADIUS;

	// LETS MAKE SOME CONSTRAINTS
	g_cloth->CreateConstraint0dof(((g_clothheight-1)*(g_clothlength)));
	g_cloth->CreateConstraint0dof(((g_clothheight)*(g_clothlength)-1));
	//g_cloth->CreateConstraint0dof(0);
	//g_cloth->CreateConstraint0dof(g_clothlength - 1);

	cur_frame = new frame((g_cloth->numberverticies));
	cur_frame->SetFrame(*(g_cloth->POS));
}

void runSimulation(void) {
	setupState();
	// go to the last frame
	printf("please wait, taking a step\n");
	while(cur_frame->nextframe != NULL) {
		cur_frame = cur_frame->nextframe;
	}
	cur_frame->UpdateToFrame(*g_cloth); // load the last frame into g_cloth.
	for(int i = 0; i<9; i++) {
		TakeStep();
		// get the next frame and add it to the end of the list of frames
		frame * temp;
		temp = new frame((g_cloth->numberverticies));
		temp->SetFrame(*(g_cloth->POS));
		cur_frame->nextframe = temp;
		temp->lastframe = cur_frame;
		cur_frame = temp;
	}
	cout << "Taken ten steps sucessfully\n";
	cout << "Now watch some convergence...\n";
}

// M A I N /////////////////////////////////////////////////////////////

// _____________________________________________________
//|														|
//|	 main												|
//|_____________________________________________________|
///
///  The main entry-point for the application

int main(int argc, char **argv) 
{  
	string answer;
	initGlutState(argc,argv);

	glewInit();								// load	the	OpenGL extensions

	// Check that	our	graphics card and driver support the necessary extensions
	if (!GLEW_VERSION_2_0)
	{
		fprintf(stderr, "Error: card/driver does not support OpenGL 2.0\n");
		do {
			std::cout << "Would you like to run a simulation anyway? (y or n)\n";
			std::cin >> answer;
		} while (!((answer.compare("y")==0) || (answer.compare("n")==0))); // compare -> 0 if equal
		if(answer.compare("y")==0)
			runSimulation();
		std::cout << "Exiting Now\n";
		system("PAUSE");
		return 1;
	}

	InitGLState();

	int shadeRet=ShaderInit("fp0.vert", "fp0.frag", h_program);
	if (!shadeRet)
	{
		fprintf(stderr, "Error: could not build the shaders\n");
		return 1;
	}

	if(InitGLHandles())
		return 1;

	setupState();

	printf("\n\t\tNOTE: Press h for help\n\n");

	// Pass control to glut to handle	the	main program loop
	glutMainLoop();

	return 0;
}
