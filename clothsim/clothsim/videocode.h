#ifndef VIDEOCODE_H // header guards
#define VIDEOCODE_H

void BeginMovie(int g_screen_width, int g_screen_height, int kstretch, int kdstretch, int kshear, 
				int kdshear, int kbend, int kdbend, int step);
void EndMovie(void);
void UpdateMovie(int g_screen_width, int g_screen_height, unsigned char *data);

#endif