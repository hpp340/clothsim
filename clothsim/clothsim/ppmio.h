/* =============================================================== */
/*
   CS175 : Computer Graphics
   Asst1 : PPM Viewer

   Name : CS175
   Date : 09/18/1997

   Filename : ppmio.h
   Description : Includes specific to PPM manipulations

*/
/* =============================================================== */

#ifndef PPMIO_H
#define PPMIO_H

typedef struct _pixel 
{
    unsigned char r,g,b;
} pixel;

/*
 * Returns an array of struct pixel (call free on it when you're done)
 * with the pixel data from the specified file. Sets width and height.
 * Returns NULL on error (no file, invalid file, etc.)
 */ 
pixel * ppmread(const char *filename, int *wp, int *hp);

/*
 * Writes the provided pixel data to the specified file. Returns -1 on error.
 */
int ppmwrite(const char *filename, int wp, int hp, pixel *data);

#endif
