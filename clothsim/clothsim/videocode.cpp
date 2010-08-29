#include "StdAfx.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <direct.h>
#include <vfw.h>
#include <assert.h>
#include <sys/stat.h>
//#include "TConvert.h"

using namespace std;

const int NUM_WINDOWS = 1;

struct VIDEODATA
{
	PAVISTREAM	  Stream;			   //--The videos stream
	bool		  VideoOK;			   //--Make sure the video process has no errors
	PAVIFILE	  VideoFile;		   //--The video file
	unsigned int  VideoWidth;	       //--The videos frame width
	unsigned int  VideoHeight;	       //--The videos frame height
	int			  NumberOfFrames;	   //--Current number of frames in the video
	AVISTREAMINFO StreamInformation;   //--The video streams information
	PAVISTREAM    CompressionStream;   //--The videos compression stream
	bool		  AllowCodecSelection; //--Allows manual selection of the codec
} g_viddat;


bool file_exists(const char *fname)
{
	struct stat sbuf;
    // exists && is a regular file
    return ((stat(fname, &sbuf) == 0));
}


//=================================================================

// Movie Stuff

void BeginMovie(int g_screen_width, int g_screen_height, int kstretch, int kdstretch, int kshear, 
				int kdshear, int kbend, int kdbend, int step)
{
	LPCTSTR input = _T("movie.avi");

	g_viddat.VideoOK				= true;
	g_viddat.AllowCodecSelection	= true;
	g_viddat.VideoWidth				= NUM_WINDOWS * g_screen_width;
	g_viddat.VideoHeight			= g_screen_height;

	AVIFileInit();
	if(AVIFileOpen(&g_viddat.VideoFile, input, OF_WRITE | OF_CREATE, NULL) != AVIERR_OK)
	{
		cout << "error: could not initialize movie" << endl;
	}
	cout << "initialized movie" << endl;

}

void EndMovie(void)
{
	if(g_viddat.Stream)
	{
		AVIStreamClose(g_viddat.Stream);
		g_viddat.Stream = NULL;
	}
	if(g_viddat.CompressionStream)
	{
		AVIStreamClose(g_viddat.CompressionStream);
		g_viddat.CompressionStream = NULL;
	}
	if(g_viddat.VideoFile)
	{
		AVIFileClose(g_viddat.VideoFile);
		g_viddat.VideoFile = NULL;
	}
	AVIFileExit();
	g_viddat.NumberOfFrames = 0;
}

void UpdateMovie(int g_screen_width, int g_screen_height, unsigned char *data)
{
	if(g_viddat.VideoOK == false)
		return;

	unsigned long BitmapSize = sizeof(BITMAPINFOHEADER) + (((g_viddat.VideoWidth * 32 + 31) / 32 * 4) * g_viddat.VideoHeight);
	BITMAPINFOHEADER header;
	header.biSizeImage		= BitmapSize - sizeof(BITMAPINFOHEADER);
	header.biSize			= sizeof(BITMAPINFOHEADER);
	header.biHeight			= g_screen_height;
	header.biWidth			= g_screen_width * NUM_WINDOWS;
	header.biCompression	= BI_RGB;
	header.biBitCount		= 32;
	header.biPlanes			= 1;
	header.biXPelsPerMeter	= 0;
	header.biYPelsPerMeter	= 0;
	header.biClrUsed	  	= 0;
	header.biClrImportant	= 0;

	if(g_viddat.NumberOfFrames == 0)
	{
		memset(&g_viddat.StreamInformation, 0, sizeof(AVISTREAMINFO));
		g_viddat.StreamInformation.rcFrame.top		    = 0;
		g_viddat.StreamInformation.rcFrame.left		    = 0;
		g_viddat.StreamInformation.fccHandler			= 0;
		g_viddat.StreamInformation.dwScale              = 1;
		g_viddat.StreamInformation.dwRate               = 60;
		g_viddat.StreamInformation.rcFrame.right		= g_viddat.VideoWidth;
		g_viddat.StreamInformation.rcFrame.bottom	    = g_viddat.VideoHeight;
		g_viddat.StreamInformation.fccType               = mmioFOURCC('v', 'i', 'd', 's');
		g_viddat.StreamInformation.dwSuggestedBufferSize = g_viddat.VideoWidth * g_viddat.VideoHeight;

		//--Create the main file stream
		if(AVIFileCreateStream(g_viddat.VideoFile, &g_viddat.Stream, &g_viddat.StreamInformation) != AVIERR_OK)
		{
			cout << "error: avi file create stream" << endl;
			return;
		}

		//--Setup the codec compression data
		AVICOMPRESSOPTIONS CompressionOptions;
		AVICOMPRESSOPTIONS FAR* CompressionOptionsF[1] = {&CompressionOptions};
		memset(&CompressionOptions, 0, sizeof(CompressionOptions));

		//--If allowing to select the codec
		if(g_viddat.AllowCodecSelection)
		{
			//--Show the dialog box to select the codec
			if(!AVISaveOptions(NULL, 0, 1, &g_viddat.Stream, (LPAVICOMPRESSOPTIONS FAR*)&CompressionOptionsF))
			{
				cout << "error: could not select codec properly" << endl;
				return;
			}
		}

		//--Make the compression stream
		if(AVIMakeCompressedStream(&g_viddat.CompressionStream, g_viddat.Stream, &CompressionOptions, NULL) != AVIERR_OK)
		{
			cout << "error: could not make compressed video stream" << endl;
			return;
		}

		//--Setup the stream format
		if(AVIStreamSetFormat(g_viddat.CompressionStream, 0, &header, header.biSize + header.biClrUsed * sizeof(RGBQUAD)) != AVIERR_OK)
		{
			cout << "error: could not set avi stream format" << endl;
			return;
		}

	}

	//--Write the current screen shot to the AVI file
	if(AVIStreamWrite(g_viddat.CompressionStream, g_viddat.NumberOfFrames++, 1, data, header.biSizeImage, AVIIF_KEYFRAME, NULL, NULL) != AVIERR_OK)
	{
		cout << "error: could not update avi frame" << endl;
		return;
	}


}


//=================================================================

/*
{ // example usage
	BeginMovie();


	if(g_movie)
	{
		static bool f1 = false;
		static bool f2 = false;
		static bool f3 = false;

		if(!g_framedat)
			g_framedat = new unsigned char[4 * g_screen_width * NUM_WINDOWS * g_screen_width];
		unsigned char *screendat = new unsigned char[4 * g_screen_width * g_screen_width];	

		glReadPixels(0, 0, g_screen_width, g_screen_height, GL_RGBA, GL_UNSIGNED_BYTE, screendat);

		int offset;
		switch(algnum)
		{
		case ALG_PLANE_TIGHT:
			offset = 2;
			f1 = true;
			break;
		case ALG_PLANE_IDENT:
			offset = 1;
			f2 = true;
			break;
		case ALG_NORMAL_TIGHT:
			offset = 0;
			f3 = true;
			break;
		default:
			break;
		}
		for(int r=0; r<g_screen_height; r++)
		{
			for(int s=0; s<g_screen_width; s++)
			{
				int xindex = offset * (g_screen_width * 4) + s*4;
				int yindex = r * (g_screen_width * NUM_WINDOWS * 4);
				int index = yindex + xindex;

				int si = r * (g_screen_width * 4) + s * 4;

				g_framedat[index] = screendat[si+2];
				g_framedat[index+1] = screendat[si+1];
				g_framedat[index+2] = screendat[si];
				g_framedat[index+3] = screendat[si+3];
			}
		}

		if(f1 && f2 && f3)
		{
			f1 = false; f2 = false; f3 = false;
			UpdateMovie(g_framedat);
		}
		
		delete [] screendat;
	}

	EndMovie();
	g_movie = false;
}*/