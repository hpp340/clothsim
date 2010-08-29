#include "StdAfx.h"
#include "cloth.h"
#include <math.h>

// CLOTH CONSTRUCTOR FUNCTION...  SETS EVERYTHING UP AND MAKES MEMORY SPACE
cloth::cloth(int _length, int _height){
	numberverticies = _length * _height;
	
	VEL =			new DiagonalMatrix(numberverticies, false);
	POS	=			new DiagonalMatrix(numberverticies, false);
	MASS =			new DiagonalMatrix(numberverticies, false);
	SMASS =			new SymMatrixBlocks(numberverticies, false);
	FORCETOTAL =	new DiagonalMatrix(numberverticies, false);
	P =				new DiagonalMatrix(numberverticies, false);
	INV_P =			new DiagonalMatrix(numberverticies, false);
	z =				new DiagonalMatrix(numberverticies, false);
	s =				new DiagonalMatrix(numberverticies, false);
	r =				new DiagonalMatrix(numberverticies, false);
	b =				new DiagonalMatrix(numberverticies, false);
	m_y =			new DiagonalMatrix(numberverticies, false);		
	dv =			new DiagonalMatrix(numberverticies, false);	
	constr_q =		new DiagonalMatrix(numberverticies, false);
	constr_p =		new DiagonalMatrix(numberverticies, false);
	c =				new DiagonalMatrix(numberverticies, false);
	TOTALdf_dx =	new SymMatrixBlocks(numberverticies, false);
	TOTALdf_dv =	new SymMatrixBlocks(numberverticies, false);
	A =				new SymMatrixBlocks(numberverticies, false);
	S =				new Matrix3x3[numberverticies];
	temp =          new DiagonalMatrix(numberverticies, false);
	temp2 =         new DiagonalMatrix(numberverticies, false);
	q =				new DiagonalMatrix(numberverticies, false);
	dmat =			new DiagonalMatrix(numberverticies, false);

	length = _length;
	height = _height;

	normals =		new DiagonalMatrix(numberverticies, false);

	symblocktemp =	new SymMatrixBlocks(numberverticies,false);
}


//create the array of triangles with a given height and width (for each triangle)
void cloth::CreateTriangleArray (void) {
	// check to see that the hight and lengths are ok;
	if ((length<2) || (height<2)) {
		printf("ERROR: cannot create triangle array with length or height less than 2\n\n");
		return;
	}
	/* get memory for the new list.  NOTE: triangles are set up in a block like this:
	   this is the BOTTOM LEFT HAND BLOCK...
					   .
					   .
					   .
	  index(length) *-----* index(length+1)
					|\ tri|		
					| \ 1 |
					|  \  |     . . .
					|tri\ |
					| 0  \|
		   index(0) *-----* index(1)
		and this repeats in a mesh*/
	int numbertri = 2*(length-1)*(height-1);
	numbertriangles = numbertri;					//this is for the draw function
	trianglelist = new triangle [numbertri];

	// now set up the array
	int i, j;
	numbertri=0;
	for (i = 0; i<(height-1); i++) {
		for (j = 0; j<(length-1); j++) {
			if(j<(length-1)) {
				//tri 0 from diagram
				trianglelist[numbertri].ai = (i*length + j);			// bottom left
				trianglelist[numbertri].ax = ((float)j / length);
				trianglelist[numbertri].ay = ((float)i / height);

				trianglelist[numbertri].bi = ((i*length) + (j+1));		// bottom right
				trianglelist[numbertri].bx = ((float)(j+1) / length);
				trianglelist[numbertri].by = ((float)i / height);

				trianglelist[numbertri].ci = (((i+1)*length) + j);		// top left
				trianglelist[numbertri].cx = ((float)j / length);
				trianglelist[numbertri].cy = ((float)(i+1) / height);
				numbertri ++;
				//tri 1 from diagram
				trianglelist[numbertri].ai = (((i+1)*length) + (j+1));	// top right
				trianglelist[numbertri].ax = ((float)(j+1) / length);
				trianglelist[numbertri].ay = ((float)(i+1) / height);

				trianglelist[numbertri].bi = (((i+1)*length) + j);		// top left
				trianglelist[numbertri].bx = ((float)j /length);
				trianglelist[numbertri].by = ((float)(i+1) / height);

				trianglelist[numbertri].ci = ((i*length) + (j+1));		// bottom right
				trianglelist[numbertri].cx = ((float)(j+1) / length);
				trianglelist[numbertri].cy = ((float)i / height);
				numbertri ++;
			} else {}			// there is no triangle pair down the end
		}
	}
	return;
}

void cloth::GetNormals(void) {
	normals->Zero();

	// first we need to get the normals per triangle
	for (int index = 0; index < numbertriangles; index++) {
		vector3d vectemp_paral1;
		vector3d vectemp_paral2;
		triangle *cur_tri;
		float tmp;

		cur_tri = &trianglelist[index];
		tmp = ((float)index)/2;
		// get the vectors vec1:B->A and vec2:B->C
		vectemp_paral1.x = (POS->m_block[cur_tri->ai].x - POS->m_block[cur_tri->bi].x);
		vectemp_paral1.y = (POS->m_block[cur_tri->ai].y - POS->m_block[cur_tri->bi].y);
		vectemp_paral1.z = (POS->m_block[cur_tri->ai].z - POS->m_block[cur_tri->bi].z);
		vectemp_paral2.x = (POS->m_block[cur_tri->ci].x - POS->m_block[cur_tri->bi].x);
		vectemp_paral2.y = (POS->m_block[cur_tri->ci].y - POS->m_block[cur_tri->bi].y);
		vectemp_paral2.z = (POS->m_block[cur_tri->ci].z - POS->m_block[cur_tri->bi].z);

		// find the normal at A
		*(cur_tri->trinormal) = (vectemp_paral1.CrossProduct(vectemp_paral2));
		cur_tri->trinormal->Normalize();
		// now add the running totals for each vertex
		normals->m_block[cur_tri->ai] += *cur_tri->trinormal;
		normals->m_block[cur_tri->bi] += *cur_tri->trinormal;
		normals->m_block[cur_tri->ci] += *cur_tri->trinormal;
	}

	// now go through and divide each vector by the correct amount
	for (int i = 0; i<numberverticies; i++) {
		normals->m_block[i].Normalize();
	}

}

// iteratively goes through the triangle list and draws each triangle.
// each triangle is drawn A->B->C which is anticlockwise.
void cloth::DrawCloth(GLuint & h_vTexCoord, bool block) {

	GetNormals();
	glShadeModel(GL_SMOOTH);
	//draw the front A->C->B   - anticlockwise
	glBegin(GL_TRIANGLES);
		for (int index = 0; index < numbertriangles; index++) {
			triangle *cur_tri;
			cur_tri = &trianglelist[index];
			//draw the front A->C->B
			// VERTEX A
			if(block == true)
				glNormal3f(cur_tri->trinormal->x,cur_tri->trinormal->y, cur_tri->trinormal->z);
			else
				glNormal3f(normals->m_block[cur_tri->ai].x, normals->m_block[cur_tri->ai].y,
						   normals->m_block[cur_tri->ai].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->ax, cur_tri->ay);   
			glVertex4f(	POS->m_block[cur_tri->ai].x,
						POS->m_block[cur_tri->ai].y,
						POS->m_block[cur_tri->ai].z, 1.0);
			// VERTEX C
			if(block == true)
				glNormal3f(cur_tri->trinormal->x,cur_tri->trinormal->y, cur_tri->trinormal->z);
			else
				glNormal3f(normals->m_block[cur_tri->ci].x, normals->m_block[cur_tri->ci].y,
						   normals->m_block[cur_tri->ci].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->cx, cur_tri->cy);   
			glVertex4f(	POS->m_block[cur_tri->ci].x,
						POS->m_block[cur_tri->ci].y,
						POS->m_block[cur_tri->ci].z, 1.0);
			// VERTEX B
			if(block == true)
				glNormal3f(cur_tri->trinormal->x,cur_tri->trinormal->y, cur_tri->trinormal->z);
			else
				glNormal3f(normals->m_block[cur_tri->bi].x, normals->m_block[cur_tri->bi].y,
						   normals->m_block[cur_tri->bi].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->bx, cur_tri->by);
			glVertex4f(	POS->m_block[cur_tri->bi].x,
						POS->m_block[cur_tri->bi].y,
						POS->m_block[cur_tri->bi].z, 1.0);

			//draw the back A->B->C   - clockwise 
			// VERTEX A
			if(block == true)
				glNormal3f(-cur_tri->trinormal->x,-cur_tri->trinormal->y, -cur_tri->trinormal->z);
			else
				glNormal3f(-normals->m_block[cur_tri->ai].x, -normals->m_block[cur_tri->ai].y,
						   -normals->m_block[cur_tri->ai].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->ax, cur_tri->ay);   
			glVertex4f(	POS->m_block[cur_tri->ai].x,
						POS->m_block[cur_tri->ai].y,
						POS->m_block[cur_tri->ai].z, 1.0);
			// VERTEX B
			if(block == true)
				glNormal3f(-cur_tri->trinormal->x,-cur_tri->trinormal->y, -cur_tri->trinormal->z);
			else
				glNormal3f(-normals->m_block[cur_tri->bi].x, -normals->m_block[cur_tri->bi].y,
						   -normals->m_block[cur_tri->bi].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->bx, cur_tri->by);
			glVertex4f(	POS->m_block[cur_tri->bi].x,
						POS->m_block[cur_tri->bi].y,
						POS->m_block[cur_tri->bi].z, 1.0);
			// VERTEX C
			if(block == true)
				glNormal3f(-cur_tri->trinormal->x,-cur_tri->trinormal->y, -cur_tri->trinormal->z);
			else
				glNormal3f(-normals->m_block[cur_tri->ci].x, -normals->m_block[cur_tri->ci].y,
						   -normals->m_block[cur_tri->ci].z);
			glVertexAttrib2f(h_vTexCoord, cur_tri->cx, cur_tri->cy);   
			glVertex4f(	POS->m_block[cur_tri->ci].x,
						POS->m_block[cur_tri->ci].y,
						POS->m_block[cur_tri->ci].z, 1.0);
		}
	glEnd();

	return;
}

// ADDED AUG 2006
void TrianglePair::SetTri(triangle * tri_in1, triangle * tri_in2, int index1, int index2) {
	tri1 = tri_in1;
	tri2 = tri_in2;
	vert1 = index1;
	vert2 = index2;
	return;
}

// This next shit finds all the triangle pairs which will be used in finding the bend fources ADDED AUG 2006
void cloth::CreateTrianglePairs(void) {
	// check to see that the hight and lengths are ok;
	if ((length<2) || (height<2)) {
		printf("ERROR: cannot create triangle array with length or height less than 2\n\n");
		return;
	}
	// find the size and make memory space
	numbertrianglepairs = (length-1)*(height-2); //verticle triangle pairs
	numbertrianglepairs += ((length-1)+(length-2))*(height-1); // horizontal and diag tri pairs
	TrianglePairArray = new TrianglePair[numbertrianglepairs];
	// Make the array
	int index = 0;
	for (int i = 0; i<height-1; i++) {
		for (int j = 0; j<length-1; j++) {
			if(i==height-2){ // On the top row of boxes
				if(j<length-2) { // Not on the top right
					//diag pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2))]; // bottom left triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
					// horiz pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 2]; // bottom left triangle over-to-right
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + (j+1)); // top right
					index ++;
				} else { // On the top right
					//diag pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2))]; // bottom left triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
				}
			} else { // Not on the top row of boxes
				if(j<length-2) { // Not down the right hand side
					//diag pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2))]; // bottom left triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
					// horiz pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 2]; // bottom left triangle over-to-right
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + (j+1)); // top right
					index ++;
					// vert pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+((i+1)*((length-1)*2))]; // bottom left triangle up
					TrianglePairArray[index].vert1 = (((i+1)*length) + (j+1)); // top right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
				} else { // Down the right hand side now
					//diag pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2))]; // bottom left triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].vert1 = ((i*length) + (j+1)); // bottom right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
					// vert pair
					TrianglePairArray[index].tri1 = & trianglelist[(j*2)+(i*((length-1)*2)) + 1]; // top right triangle
					TrianglePairArray[index].tri2 = & trianglelist[(j*2)+((i+1)*((length-1)*2))]; // bottom left triangle up
					TrianglePairArray[index].vert1 = (((i+1)*length) + (j+1)); // top right
					TrianglePairArray[index].vert2 = (((i+1)*length) + j); // top left
					index ++;
				}
			}
		}
	}
	return;
}


void VertexPair::SetVert(int input1, int input2, bool dia, bool horiz, bool ben) {
	vert1 = input1;
	vert2 = input2;
	diag = dia;
	horizontal = horiz;
	bend = ben;
	return;
}

/*	this function is used when finding the stretch and sheer forces.  The layout of the data structure
	of VertexPairArray is similar to triangles except that we add another diagonal...  And we differeniate
	them with the boolian diag...  These diags will make up the shear forces.
					   .
					   .
					   .
	  index(length) *-----* index(length+1)
					|\   /|		
					| \ / |
					|  \  |     . . .
					| / \ |
					|/   \|
		   index(0) *-----* index(1) */
void cloth::CreateVertexPairs(void) {
	// check to see that the hight and lengths are ok;
	if ((length<2) || (height<2)) {
		printf("ERROR: cannot create triangle array with length or height less than 2\n\n");
		return;
	}

	// find the size and make memory space
	// count vertexpairs along perim. first: bottom + LHS + top + RHS
	numbervertexpairs = ((length-2)*4) + ((height-1)*3) + ((length-1)*1) + ((height-1)*2);
	// now count vertexpairs on inside
	numbervertexpairs += ((length-2)*(height-2)*4);
	// now count bend pairs. LHS + (LHS + 1) + MIDDLE + (RHS - 1) + RHS
	numbervertexpairs += (((height-2)*3) + 2 + ((height-2)*3) + 2 + ((length-4)*(height-2)*4) + 
						   ((length-4)*2) + ((height-2)*2) + ((height-2)*2));
	VertexPairArray = new VertexPair[numbervertexpairs];
	
	//now iterate through putting in the right vertex indicies.  (Right diag goes up to the right)
	int index = 0;
	for (int i = 0; i<height; i++) {
		for (int j = 0; j<length; j++) {
			if (i == (height-1)) {					// on the top
				if(j<(length-2)) {					// not TOP - 2
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;				
				} else if(j!=(length-1)) {					// not TOP RIGHT
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
				}
			} else if (i == (height-2)) {			// TOP - 1
				if (j == 0) {						// LHS
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;
				} else if (j<(length-2)) {			// >LHS
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;
				} else if(j==(length-2)) {			// RHS-1
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;				
				} else {							// RHS
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;	
				}
			} else {	// <TOP - 2
				if(j==0) {							// LHS
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j+2), true, false, true);	//Right diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j), false, false, true);		//vert bend
					index++;					
				} else if (j == 1) {				// along the LHS + 1
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j+2), true, false, true);	//Right diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j), false, false, true);		//vert bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;		
				} else if (j<(length-2)) {			// MIDDLE
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+2), false, true, true);		//horiz bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j+2), true, false, true);	//Right diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j), false, false, true);		//vert bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j-2), true, false, true);	//Left diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;
				} else if(j<(length-1)) {			// RHS + 1
					VertexPairArray[index].SetVert(((i*length)+j), ((i*length)+j+1), false, true, false);		//horiz
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j+1), true, false, false);	//Right diag
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j), false, false, true);		//vert bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j-2), true, false, true);	//Left diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;
				} else if(j==(length-1)) {			// RHS
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j), false, false, false);	//vert
					index++;	
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j), false, false, true);		//vert bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+2)*length)+j-2), true, false, true);	//Left diag bend
					index++;
					VertexPairArray[index].SetVert(((i*length)+j), (((i+1)*length)+j-1), true, false, false);	//Left diag
					index++;
				}
			}
		}
	}
	return;
}

void cloth::CreateConstraint3dof(int index) 
{
	S[index].Ident();
}

void cloth::CreateConstraint2dof(int index, vector3d p)
{
	S[index].Ident();
	S[index] -= p.CreateMat();
}
void cloth::CreateConstraint1dof(int index, vector3d p, vector3d q) // NOTE: p and q MUST be orthogonal
{
	S[index].Ident();
	S[index] -= p.CreateMat();
	S[index] -= q.CreateMat();
}
void cloth::CreateConstraint0dof(int index)
{
	S[index].Zero();
}

void cloth::SetS(void)
{
	for(int i = 0; i < numberverticies; i++)
	{
		S[i].Ident();
	}
}

void cloth::Filter(DiagonalMatrix &src, DiagonalMatrix &ret)
{
	// Multiply src by constraint matrix
	for(int i = 0; i < numberverticies; i++)
	{
		ret.m_block[i] = S[i] * src.m_block[i];
	}
}
void cloth::InitCloth(float xsize, float ysize, int mass) {
	// check to see that the hight and lengths are ok;
	if ((length<2) || (height<2)) {
		printf("ERROR: cannot create triangle array with length or height less than 2\n\n");
		return;
	}

	//set masses and constraints
	for (int index = 0; index < numberverticies; index++) {
		// we need to set both the Mass matrix and SMass matrix to have finite mass
		MASS->m_block[index].x	= mass;
		MASS->m_block[index].y	= mass;
		MASS->m_block[index].z	= mass;
		(*SMASS)(index, index)->Ident();  //(ie, masses of 1)
		// set all verticies to be un-constrained (with 3 d.o.f)
		S[index].Ident();

		//  This adds force for when it goes through the floor
		m_y->m_block[index].x = 0;
		m_y->m_block[index].y = 0;
		m_y->m_block[index].z = 0;
	}

	// Create the Triangle array with texture coordinates for drawing, and vertex pair array for forces
	CreateTriangleArray();
	CreateVertexPairs();
	CreateTrianglePairs();  // NEWLY ADDED AUG 2006
	SetS();			// set the constraint matrix to 3dof for all verticies
	z->Zero();		// set the z matrix so we start at zero on every iteration
	int i, j;

	// set velocities to zero (all at rest to start!)
	for (i = 0; i<height; i++) {
		for (j = 0; j<length; j++) {
			VEL->m_block[(i*length + j)].x = 0.0;
			VEL->m_block[(i*length + j)].y = 0.0;
			VEL->m_block[(i*length + j)].z = 0.0;
		}
	}

	// set positions (cloth will live in the XZ plane)
	float lengthoffset = (0.5 * xsize * length);
	float heightoffset = (0.5 * ysize * length);
	for (i = 0; i<height; i++) {
		for (j = 0; j<length; j++) {
			POS->m_block[(i*length + j)].x = (float) (j * xsize) - lengthoffset;
//			POS->m_block[(i*length + j)].y = (float) (i * ysize) - heightoffset;
			POS->m_block[(i*length + j)].y = 0.0;
//			POS->m_block[(i*length + j)].z = 0.0;
			POS->m_block[(i*length + j)].z = (float) (i * ysize) - heightoffset;
		}
	}

	// this will happen at the start of the next iteration anyway, but for good measure, reset the forces
	ResetForces();
	sucess = 0;
	hstep_increase_bound = 2;
	stepTooLarge = false;

	return;
}

void cloth::GetFStretchOrShear (int index0, int index1, float natlength, float k, 
							   float kd, SymMatrixBlocks &totaldf_dx, SymMatrixBlocks &totaldf_dv) {

	// first get a handler to the positions and the velocities of each vertex in question.
	pos0 = &(POS->m_block[index0]);
	pos1 = &(POS->m_block[index1]);
	vel0 = &(VEL->m_block[index0]);
	vel1 = &(VEL->m_block[index1]);

	// Now find C"	This is equal to ((x2-x1)^2 + (y2-y1)^2 + (z2-z1)^2)^1/2 - 
	// "natural rest length", According to Baraff and Witkin...
	mag = (pos1->x-pos0->x)*(pos1->x-pos0->x) + 
		  (pos1->y-pos0->y)*(pos1->y-pos0->y) + 
		  (pos1->z-pos0->z)*(pos1->z-pos0->z);
	mag = sqrt(mag);
	C = (mag - natlength);

	// hopefully this should get rid of roundoff errors!
	if(C < (5e-7))
		C = 0.0;

	// need to find dC_dx.  This is non other than the unit vector that points from vertex1
	// to vertex2.  It will however, be negetive for one of the verticies.  Therefore, we just
	// normalize the vector...  (1/magnitude) * <x2-x1, y2-y1, z2-z1>
	if (mag == 0.0)
		mag = (float)1e-15;		//condition means that if we are overlapping, dc_dx should be V.small
	inv_mag = 1.0/mag;			//precalculate to save calc. time
	dC_dx[1].x = inv_mag * (pos1->x-pos0->x);
	dC_dx[1].y = inv_mag * (pos1->y-pos0->y);
	dC_dx[1].z = inv_mag * (pos1->z-pos0->z);
	dC_dx[0] = (-1.0) * dC_dx[1];

	// Now we're ready to work out the forces.  The equation is: ftotal = finternal + fdamp
	// where finternal = -k * dC(x)/dx * C(x) and fdamp = -kd * dC(x)/dx * Cdot(x) (eqations 7 & 11).
	// note: Cdot = dotproduct [dC/dx , (velocity2-velocity1)]
	Cdot = (dC_dx[0].x * vel0->x) + (dC_dx[0].y * vel0->y) + (dC_dx[0].z * vel0->z);
	Cdot += (dC_dx[1].x * vel1->x) + (dC_dx[1].y * vel1->y) + (dC_dx[1].z * vel1->z);
	force0 = (-k * C * dC_dx[0]) - (kd * dC_dx[0] * Cdot);
	force1 = (-k * C * dC_dx[1]) - (kd * dC_dx[1] * Cdot);

	// now add this new force contribution to overall force.
	FORCETOTAL->m_block[index0] += force0;
	FORCETOTAL->m_block[index1] += force1;

	// We will also need  df/dx (which is a matrix).  This is taken from the equation:
	// Kij = df/dx = -k * (dC/dxi * Transpose(dC/dxj) + d2C/dxixj * C).  This is an approx. since we're
	// dropping a second dampening term, to preserve symetry.  (top left of page 48)
	// Therefore, first we need the second derivatives.  These are taken from the equation:
	// d2C /(dx1dx2) = -1/mag^3 * (x2-x1)^2 + 1/mag
	// which can be adapted to d2C/dx1dx1, d2C/dx2dx2, d2C/dx2dx1 by subing in.
	inv_mag_cubed = inv_mag*inv_mag*inv_mag;			//precalculate to save calc. time
	d2C_dxdx[0][1].x = (inv_mag_cubed*((pos1->x-pos0->x)*(pos1->x-pos0->x))) - inv_mag;
	d2C_dxdx[0][1].y = (inv_mag_cubed*((pos1->y-pos0->y)*(pos1->y-pos0->y))) - inv_mag;
	d2C_dxdx[0][1].z = (inv_mag_cubed*((pos1->z-pos0->z)*(pos1->z-pos0->z))) - inv_mag;
	d2C_dxdx[1][0] = d2C_dxdx[0][1];
	d2C_dxdx[0][0].x = (-inv_mag_cubed*((pos1->x-pos0->x)*(pos1->x-pos0->x))) + inv_mag;
	d2C_dxdx[0][0].y = (-inv_mag_cubed*((pos1->y-pos0->y)*(pos1->y-pos0->y))) + inv_mag;
	d2C_dxdx[0][0].z = (-inv_mag_cubed*((pos1->z-pos0->z)*(pos1->z-pos0->z))) + inv_mag;
	d2C_dxdx[1][1] = d2C_dxdx[0][0];

	// Remember, second derivatives are diagonal which makes things easy for this next part
	int index[2];
	index[0] = index0;
	index[1] = index1;
	int nn;
	for (int i=0; i<2; i+=1) {
		for(int j=i; j<2; j+=1) {
			mattmp = dC_dx[i].CreateMat(dC_dx[j]);		// this will create a matrix with values i*j
			df_dx.elt[0] = (mattmp.elt[0] + d2C_dxdx[i][j].x * C);
			df_dx.elt[0] = (df_dx.elt[0] * (-k)) - (kd * d2C_dxdx[i][j].x * Cdot);
			df_dx.elt[1] = -k * mattmp.elt[1];
			df_dx.elt[2] = -k * mattmp.elt[2];
			df_dx.elt[3] = -k * mattmp.elt[3];
			df_dx.elt[4] = (mattmp.elt[4] + d2C_dxdx[i][j].y * C);
			df_dx.elt[4] = (df_dx.elt[4] * (-k)) - (kd * d2C_dxdx[i][j].y * Cdot);
			df_dx.elt[5] = -k * mattmp.elt[5];
			df_dx.elt[6] = -k * mattmp.elt[6];
			df_dx.elt[7] = -k * mattmp.elt[7];
			df_dx.elt[8] = (mattmp.elt[8] + d2C_dxdx[i][j].z * C);
			df_dx.elt[8] = (df_dx.elt[8] * (-k)) - (kd * d2C_dxdx[i][j].z * Cdot);
			// add in contribution to the right spot
			(*(totaldf_dx(index[i], index[j]))) += df_dx;

			// Now we need to work out the df_dv component and add that too: dC/dv = d/dv transpose(dC/dx)*v
			for (nn = 0; nn<9; nn+=1) {
				df_dv.elt[nn] = -kd * mattmp.elt[nn];
			}
			// add in contribution to the right spot
			(*(totaldf_dv(index[i], index[j]))) += df_dx;
		}
	}
}

// NEED TO FINISH THIS LATER
void cloth::GetFBend (TrianglePair *input, float k, float kd, SymMatrixBlocks &totaldf_dx, SymMatrixBlocks 
					  &totaldf_dv) {

}

void cloth::GetFGravity(int index0, float grav) {
	// just add the force to the running total:  F = m * a.  And we're only concerned with y, ie down, direction
	FORCETOTAL->m_block[index0].y += (MASS->m_block[index0].y * (-grav));
	return;
}

void cloth::GetFTotal(float xnatlength, float ynatlength, float diagnatlength, float benddiagnatlength, float kstretch, 
					  float kdstretch, float kshear, float kdshear, float kbend, float kdbend, float grav) {

	// go though each vertex pair and calculate the stretch and shear forces
	for (int index = 0; index<numbervertexpairs; index++) {
		//STRETCH FORCE - HORIZONTAL
		if(VertexPairArray[index].diag == false && 
		   VertexPairArray[index].horizontal == true &&
		   VertexPairArray[index].bend == false) {
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  xnatlength, kstretch, kdstretch, *TOTALdf_dx, *TOTALdf_dv);
		//STRETCH FORCE - VERTICAL
		} else if(VertexPairArray[index].diag == false && 
			      VertexPairArray[index].horizontal == false &&
				  VertexPairArray[index].bend == false) {
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  ynatlength, kstretch, kdstretch, *TOTALdf_dx, *TOTALdf_dv);
		//SHEAR FORCE. NOTE:	This implimentation of shearing is DIFFERNT to Baraff and Witkin's.  In their.
		//						paper they calculated the area of the triangle and used a continuous function
		//						w such that a force c was set up so that the area of the triangle would have
		//						a "rest area".  Our implimentation really does exactly the same thing.
		//			   NOTE2:	the two verticies form a diagonal and therfore need a different natural length
		} else if(VertexPairArray[index].diag == true && 
			      VertexPairArray[index].horizontal == false &&
				  VertexPairArray[index].bend == false) {		
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  diagnatlength, kshear, kdshear, *TOTALdf_dx, *TOTALdf_dv);
		//BEND FORCE - RIGHT & LEFT DIAG
		} else if(VertexPairArray[index].diag == true && 
			      VertexPairArray[index].horizontal == false &&
				  VertexPairArray[index].bend == true) {		
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  (benddiagnatlength), kbend, kdbend, *TOTALdf_dx, *TOTALdf_dv);
		//BEND FORCE - VERT
		} else if(VertexPairArray[index].diag == false && 
			      VertexPairArray[index].horizontal == false &&
				  VertexPairArray[index].bend == true) {
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  (2*ynatlength), kbend, kdbend, *TOTALdf_dx, *TOTALdf_dv);
		//BEND FORCE - HORIZONTAL
		} else if(VertexPairArray[index].diag == false && 
			      VertexPairArray[index].horizontal == true &&
				  VertexPairArray[index].bend == true) {
			GetFStretchOrShear(VertexPairArray[index].vert1, VertexPairArray[index].vert2,
							  (2*xnatlength), kbend, kdbend, *TOTALdf_dx, *TOTALdf_dv);
		} else {
			// we shouldn't get here!!
			printf("\nERROR: Something went wrong in GetFTotal\n\n");
			exit(1);
		}
	}

	// DO LATER... THE BARIFF AND WITKIN IMPLIMENTATION, PAIRS ARE THERE JUST NEED
	// TO IMPLIMENT IT
	// go though each triangle pair and calculate the bend forces
	//for (int index = 0; index<numbertrianglepairs; index++) {
	//	GetFBend(&TrianglePairArray[index], kbend, kdbend, *TOTALdf_dx, *TOTALdf_dv);
	//}
	
	// go through each vertex and add a gravity force
	for (int index = 0; index<numberverticies; index++) {
		GetFGravity(index, grav);
	}
}

void cloth::ResetForces(void) {
	if(FORCETOTAL == NULL || TOTALdf_dx == NULL || TOTALdf_dv == NULL) {
		printf("\nERROR: CANNOT FIND FORCE (or derivatives) MATRIX IN RESET FORCES\n quiting...\n\n");
		exit(1);
	}
	FORCETOTAL->Zero();
	TOTALdf_dx->Zero();
	TOTALdf_dv->Zero();
}


// ADDED LATER SO THAT THE PAGEFILE DIDN'T BLOW UP
SymMatrixBlocks* cloth::Mult(double lhs, SymMatrixBlocks &rhs) {
	rhs.MakeTemp();
	*rhs.temp *= lhs;
	return (rhs.temp);
}

SymMatrixBlocks* cloth::Mult(SymMatrixBlocks &lhs, double rhs) 
{
	lhs.MakeTemp();
	*lhs.temp *= rhs;
	return (lhs.temp);
}

SymMatrixBlocks* cloth::Add(SymMatrixBlocks &lhs, SymMatrixBlocks &rhs)
{
	lhs.MakeTemp();
	*lhs.temp += rhs;
	return (lhs.temp);
}

SymMatrixBlocks* cloth::Sub(SymMatrixBlocks &lhs, SymMatrixBlocks &rhs)
{
	lhs.MakeTemp();
	*lhs.temp -= rhs;
	return (lhs.temp);
}


DiagonalMatrix* cloth::Mult(double lhs, DiagonalMatrix &rhs)
{
	rhs.MakeTemp();
	*rhs.temp *= lhs;
	return (rhs.temp);
}

DiagonalMatrix* cloth::Mult(DiagonalMatrix &lhs, double rhs)
{
	lhs.MakeTemp();
	*lhs.temp *= rhs;
	return (lhs.temp);
}

DiagonalMatrix* cloth::Add(DiagonalMatrix &lhs, DiagonalMatrix &rhs)
{
	lhs.MakeTemp();
	*lhs.temp += rhs;
	return (lhs.temp);
}

DiagonalMatrix* cloth::Sub(DiagonalMatrix &lhs, DiagonalMatrix &rhs)
{
	lhs.MakeTemp();
	*lhs.temp -= rhs;
	return (lhs.temp);
}