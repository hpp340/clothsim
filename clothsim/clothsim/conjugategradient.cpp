#include "stdafx.h"
#include "MathTypes.h"
#include "cloth.h"
#include <math.h>
#include "GLOBALS.h"

// B A R I F F   &   W I T K I N ///////////////////////////////////////////////

// this is the master function that will preform 1 iterative step
void TakeStep(void) {
	int numTries = 0;
	// reset, then work out the forces for each cloth
	g_cloth->ResetForces();
	g_cloth->GetFTotal(XNATLENGTH, YNATLENGTH, DIAGNATLENGTH, BENDDIAGNATLENGTH, KSTRETCH, KDSTRETCH, 
		KSHEAR, KDSHEAR, KBEND, KDBEND, GRAVITY);
	do{
		// NOW SOLVE!!
		if(EULERSTEP)
			g_cloth->Euler(HSTEP);
		else
			g_cloth->ConjugateGradient(ERRORTOLERANCE, HSTEP);
	} while ( g_cloth->checkStep(numTries) ); // if it is unstable reduce Tstep and try again.

	// Update the positions and velocities (ie, do a "linear step" in velocity) --> Euler
	*(g_cloth->VEL) += *(g_cloth->dv); // UPDATE VELOCITY
	*(g_cloth->temp) = *g_cloth->Mult(*(g_cloth->VEL), HSTEP); // TEMP = VECOLICTY * TIMESTEP
	*(g_cloth->POS) += *(g_cloth->temp); // UPDATE POSITION

	// Check constraints
	if(SPHERECONTACT)
		g_cloth->CheckConstraints(SPHERE_X, SPHERE_Y, SPHERE_Z, SPHERERADIUS);

	// REMAKE THE TOP CONSTRAINTS (top two)
	g_cloth->CreateConstraint0dof(((g_clothheight-1)*(g_clothlength)));
	g_cloth->CreateConstraint0dof(((g_clothheight)*(g_clothlength)-1));
	//g_cloth->CreateConstraint0dof(0);
	//g_cloth->CreateConstraint0dof(g_clothlength - 1);

}

// NOTE: ACCORDING TO PAPER THIS SHOULD CHECK STRETCH DISPLACEMENT.  NOT JUST DELTA X.
bool cloth::checkStep(int numTries) // O(n) --> A little slow, but ok.
{ 
	if(ADPATIVE_TIME_STEP)
	{
		vector3d delX; // temporary variables
		for(int index = 0; index < numberverticies; index++)
		{
			delX.x = (g_cloth->VEL->m_block[index].x + g_cloth->dv->m_block[index].x)*HSTEP; // x = (v0+delta_v).t
			delX.y = (g_cloth->VEL->m_block[index].y + g_cloth->dv->m_block[index].y)*HSTEP;
			delX.z = (g_cloth->VEL->m_block[index].z + g_cloth->dv->m_block[index].z)*HSTEP;
			if(sqrt(delX.x*delX.x+delX.y*delX.y+delX.z+delX.z) > DELTAX_TOLLERENCE) // Detected potential instability!!
			{
				HSTEP /= 2;
				if(numTries == 0 && g_cloth->hstep_increase_bound < 32 )
					g_cloth->hstep_increase_bound *= 2;
				g_cloth->stepTooLarge = true;
				g_cloth->sucess = 0;
				std::cout << "Reducing timestep by 1/2. HSTEP = " << HSTEP << "\n";
				return 1;
			}
		}
		if(numTries == 0)
			g_cloth->sucess ++;
		if(sucess > g_cloth->hstep_increase_bound)
		{
			// check if there wasn't an error between the last increase.
			if(g_cloth->stepTooLarge == false && g_cloth->hstep_increase_bound > 2)
				g_cloth->hstep_increase_bound /= 2;
			HSTEP = HSTEP*2;
			g_cloth->sucess = 0;
			g_cloth->stepTooLarge = false;
			std::cout << "Increasing timestep by 2x. HSTEP = " << HSTEP << "\n";
		}
		return 0;
	}
	else
	{
		// Assume the timestep was ok.
		g_cloth->sucess = 0;
		g_cloth->stepTooLarge = false;
		return 0;
	}
}

void cloth::Euler(float g_hstep)
{
	MASS->Invert(*temp); // invert mass and put into dv
	Filter(*temp,*temp2); // Use mass modifier matrix S to make some mass inverse values 0;
	temp2->Mult(*FORCETOTAL,*dv); // a = FORCE * M^-1
}

void cloth::ConjugateGradient(float error, float g_hstep)
{
	float hstep = g_hstep; 
	//if(reducehstep) {       // there will be high velocities during colision (moving point out)
	//	hstep = hstep / 1000; // NOTE: NOW DOING ADAPTIVE TIME STEPS!!
	//}

	// Set up A = (M - h*df/dv - h^2 * df/dx)  EQUATION 15
	*symblocktemp = *Sub(*SMASS, *(Mult(hstep, (*TOTALdf_dv))));
	(*A) = *Sub(*symblocktemp, *(Mult((hstep * hstep), (*TOTALdf_dx))));

	// Set up b = h * (FORCE + df/dx * (y + (h * VELOCITY)))  EQUATION 15
	(*temp) = *Mult(hstep, *VEL);
	(*dmat)	= *Add(*temp, *m_y);
	TOTALdf_dx->SetDiag(*dmat, *temp);
	*dmat = *Add(*FORCETOTAL, *temp);
	*b = *Mult(hstep, *dmat);

	// Set up preconditioning --> Precondition with diagonal entries of A
	for(int index = 0; index < numberverticies; index++)
	{
		INV_P->m_block[index].x = (float)(*A)(index,index)->elt[0];
		INV_P->m_block[index].y = (float)(*A)(index,index)->elt[4];
		INV_P->m_block[index].z = (float)(*A)(index,index)->elt[8];
	}
	INV_P->Invert(*P);

	// STEP 2
	*dv = *z; // z is just a zero matrix.

	// STEP 3    delta0 = filter(b)^t * P * filter(b)
	Filter(*b, *temp);
	(*P).Mult(*temp, *dmat);
	float delta_not = temp->DotProductDiagonals(*dmat);

	// STEP 4    r = filter( b - A * dv )
	(*A).SetDiag(*dv, *dmat);
	*temp = *Sub(*b, *dmat);
	Filter(*temp, *r);

	// STEP 5    c = filter( PInv * r )
	(*INV_P).Mult(*r, *temp);
	Filter(*temp, *c);

	// STEP 6    delta_new = r^t * c
	float delta_new = r->DotProductDiagonals((*c));

	float delta_old;	// for later
	// NEXT LINE TAKEN FROM HAMILTON'S CODE: looping maximum value that worked for Dean Macri is:
	double no_iterations = 0;
	
	// double max_iterations = 15*((double)sqrt((double)numberverticies)*3 + 3);

	double max_iterations = 100*((double)sqrt((double)numberverticies)*3 + 3);

	// STEP 7
	while ((delta_new > (error*error*delta_not)) && (no_iterations < max_iterations)) {
		
		// STEP 8         	q = filter( A*c )
		A->SetDiag(*c, *temp);
		Filter(*temp, *q);

		// STEP 9           alpha = delta_new / (c^t * q)
		float alpha = delta_new / (c->DotProductDiagonals((*q)));

		// STEP 10          deltav = deltav + alpha*c
		*temp = *Mult(*c, alpha);
		*dv += *temp;

		// STEP 11          r = r - alpha * q
		*temp = *Mult(*q, alpha);
		*r -= *temp;

		// STEP 12          s = PInv * r
		(*INV_P).Mult(*r, *s);

		// STEP 13
		delta_old = delta_new;

		// STEP 14          delta_new = r^t * s
		delta_new = r->DotProductDiagonals(*s);

		// STEP 15          c = filter( s + delta_new/delta_old * c )
		*dmat = *Mult(*c, (delta_new / delta_old));
		*temp = *Add(*dmat, *s); 
		Filter(*temp, *c);

		no_iterations++;
	}
	if(no_iterations == max_iterations) {
		printf("max reached\n");
	}
}

void cloth::CheckConstraints(float x, float y, float z, float rad)
{
	float norm, oneover;
	vector3d center, outside;
	center.x = x; center.y = y; center.z = z;
	// reset the reduction process
	// reducehstep = false; // NOTE: NOW DOING ADAPTIVE TIME STEPS
	// Go through each vertex and make sure it hasn't gone through the sphere
	for(int index = 0; index < numberverticies; index++)
	{
		// find the distance to the center
		vector3d *pos = &(POS->m_block[index]);
		vector3d diff = (*pos) - center;
		norm = sqrt(diff.x*diff.x+diff.y*diff.y+diff.z*diff.z);
		if(norm==0){
			// this really shouldn't be.  
		} else {
			if(norm < rad) {	// WITHIN THE SPHERE!
				// reducehstep = true; // NOTE: NOW DOING ADAPTIVE TIME STEPS
				oneover = 1.0/norm;
				diff *= oneover;  // normalize vector
				outside = diff * rad;
				POS->m_block[index] = outside + center;

				//http://mathworld.wolfram.com/VectorSpaceProjection.html
				
				VEL->m_block[index] -= diff * (diff * VEL->m_block[index]);

				// create or make a constraint
				CreateConstraint2dof(index, diff);
			} else {
				m_y->m_block[index].y = 0;
				CreateConstraint3dof(index);
			}
		}
	}
}