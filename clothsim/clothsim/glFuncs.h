
#ifndef GLFUNCS_H // header guards
#define GLFUNCS_H

void		setMatrices(void);
void		drawStuff(void);
void		display(void);
void		reshape(int w, int h);
void		mouse(int button, int state, int x, int y);
static void mousemove(int x, int y);
void		quit (void);
void		KeyFunc(unsigned char key, int x, int y);
void		initGlutState(int argc, char **argv);
void		InitGLState(void);
int			InitGLHandles(void);

#endif