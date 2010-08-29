/* =============================================================== */
/*
   CS175 : Computer Graphics
   Asst1 : PPM Viewer

   Name : CS175
   Date : 09/18/1997

   Filename : ppmio.c.reverse
   Description : PPM file manipulation functions (read & write)
   Note : This file reads and writes the PPM data and stores
          it in the appropriate array in row-reverse order.

*/
/* =============================================================== */

#include "StdAfx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ppmio.h"

/* =============================================================== */
/* 
   static int read_integer (FILE *f, int *val)

   Read one integer from a (text) file. Line beginning with
   "#" are ignored as comments.
*/

static int 
read_integer(FILE *f, int *val) 
{
    unsigned char ch;
    int got = 0, done = 0, accum = 0, incomment = 0;

    while (!done) {
	ch = fgetc(f);

	if (feof(f)) return -1;
	if (incomment) {
	    if (ch=='\n') incomment=0;
	    continue;
	}

	if (isdigit(ch)) {
	    accum = accum*10 + ch-'0';
	    got = 1;
	}
	else if (ch=='#') incomment=1;
	else if (!ch || !strchr(" \t\r\n", ch)) {
	    /* illegal character */
	    fprintf(stderr, "ppmread: invalid character 0x%02x\n", ch);
	    return -1;
	}
	else if (!got) /* nothing */ ;
	else done = 1;
    }

    *val = accum;
    return 0;
}


/* =============================================================== */
/* 
   static int ppm_readheader (FILE *f, int *width, int *height)

   Read the PPM header from file and initialize the width and height
   pointers to the appropriate values.
*/

static int 
ppm_readheader(FILE *f, int *width, int *height) 
{
    int maxcolor;
    
    if (read_integer(f, width)) return -1;
    if (read_integer(f, height)) return -1;
    if (read_integer(f, &maxcolor)) return -1;
    
    if (maxcolor!=255) {
	fprintf(stderr, "Warning: maxcolor not 255 : won't work well\n");
    }
    return 0;
}


/* =============================================================== */
/* 
   pixel * ppmread (char *filename, int *width, int *height)

   Reads the actual PPM data and stores returns in in a width * height
   size array of pixel structs.
*/

pixel *
ppmread(const char *filename, int *width, int *height) 
{
    char buf[BUFSIZ];
    int isbinary, size, l = 0, cnt = 0;
    pixel *rv = NULL;

    FILE *f = fopen(filename, "rb");
    if (!f) {
	perror("ppmread: fopen");
	return NULL;
    }
    
    if (fread(buf, 1, 2, f)!=2) {
	perror("ppmread: fread");
	fclose(f);
	return NULL;
    }
    
    if (!memcmp(buf, "P3", 2)) {
	isbinary = 0;
    }
    else if (!memcmp(buf, "P6", 2)) {
	isbinary = 1;
    }
    else {
	fprintf(stderr, "ppmread: bad file format\n");
	fclose(f);
	return NULL;
    }
    
    if (ppm_readheader(f, width, height)) {
	fprintf(stderr, "ppmread: corrupt ppm header\n");
	fclose(f);
	return NULL;
    }
    
    size = *width * *height * sizeof(pixel);
    rv = (pixel *)malloc(size);
    if (!rv) {
	perror("ppmread: malloc");
	fclose(f);
	return NULL;
    }
    
    memset(rv, 0, size);  /* clear it in case we get a partial read */
    
    if (isbinary) {
        int row;

	for (row = ((*height) - 1); row >= 0; row--) {
	  l = fread(&rv[row * (*width)], 1,
		    ((*width) * sizeof(pixel)), f);

	  if (l<0) {
	    perror("ppmread: fread");
	    fclose(f);
	    free(rv);
	    return NULL;
	  }

	  cnt += l;
	}
    }
    else {
        int row;

	for (row = ((*height) - 1); row >= 0; row--) {
	  for (l = 0; l < (*width); l++) {
	    int x;
	    if (read_integer(f, &x)) break;
	    /* This is slightly tacky. */
	    ((unsigned char *)rv)[row * (*width) + l] = x;
	  }

	  cnt += l;
	}
    }

    fclose(f);
    
    if (cnt!=size) {
	fprintf(stderr, "ppmread: Short count (%d, should be %d), file is\n",
		l, size);
	fprintf(stderr, "probably corrupt.\n");
	/* return it anyway */
    }
    
    return rv;
}


/* =============================================================== */
/* 
   int ppmwrite (char *filename, int width, int height, pixel * data)

   Writes the PPM data to file from an array of pixel structs

   [Should probably do more error checking]
*/

int 
ppmwrite(const char *filename, int width, int height, pixel *data) 
{
    const char *s;
    int row;

    FILE *f = fopen(filename, "wb");
    if (!f) {
	perror("ppmwrite: fopen");
	return -1;
    }
    
    s = strrchr(filename, '/');
    if (!s) s = filename;
    
    fprintf(f, "P6\n");
    fprintf(f, "# ppm file %s written by ppmio.c\n", s);
    fprintf(f, "%d %d 255\n", width, height);

    for (row = (height - 1); row >= 0; row--)
      fwrite(&data[row * width], 1,
	     (width * sizeof(pixel)), f);

    fclose(f);
    
    return 0;
}

