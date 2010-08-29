/////////////////////////////////////////////////////////////////////////////
//
//	shader.cpp
//
//	CS 175: Computer Graphics
//	Harvard University
//
//	functions for handling shaders
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <glew.h>
#include <glut.h>


// F I L E   H A N D L I N G ///////////////////////////////////////////


static int getFileLength(FILE *fp){
  int len = 0;
  
  rewind(fp);
  while(fgetc(fp)!=EOF)
    len++;
  rewind(fp);
  return len;
}


static int GetShaderSize(const char *filename)
{
  FILE *fd;
  int count = -1;
  
  fd = fopen(filename, "r");
  if (fd)
    {
      count = getFileLength(fd)+1;
      fclose(fd);
    }
  
  return count;
}

static int LoadShader(const char *filename, int shader_size, GLchar *shader)
{
  FILE *fd;
  int count = 0;
  int success = 1;
  
  fd = fopen(filename, "r");
  if (!fd)
    {
      printf("Error: could not open file %s for loading shader\n", filename);
      return 0;
    }
  
  fseek(fd, 0, SEEK_SET);
  count = (int) fread(shader, 1, shader_size, fd);
  shader[count] = '\0';
  
  if (ferror(fd))
    success = 0;
  
  fclose(fd);
  return success;
}

static void printInfoLog(GLuint  obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;
  

  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
			    &infologLength);


  if (infologLength > 0)
    {
      infoLog = new char [infologLength];   //(char *)malloc(infologLength);
      glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
      printf("\nInfoLog:\n--------------\n%s\n",infoLog);
      delete infoLog;
    }
}

static int LoadShaderFromFile(const char *filename, GLchar **shader)
{
	int shader_size = GetShaderSize(filename);
	if (shader_size < 0)
	{
		printf("Error: Cannot determine size of shader %s\n", filename);
		return false;
	}

	*shader = new GLchar [shader_size];  //(GLchar *)malloc(shader_size);
	if (!LoadShader(filename, shader_size, *shader))
	{
		printf("Error: Cannot parse the shader file %s\n", filename);
		return false;
	}

	return true;
}

bool ShaderInit(const char *vertexfile, const char *fragfile, GLuint &h_program)
{
	GLchar *vertex_shader		= 0;	/// vertex shader
	GLchar *frag_shader			= 0;	/// fragment (pixel) shader
	GLuint	h_vertex_shader;			/// handle to the vertex shader
	GLuint	h_frag_shader;				/// handle to the fragment (pixel) shader
	GLint	vertex_compiled;			/// status of vertex shader compilation
	GLint	frag_compiled;				/// status of fragment shader compilation
	GLint	prog_linked;				/// status of linking

	// Load the vertex and fragment shaders
	LoadShaderFromFile(vertexfile, &vertex_shader);
	LoadShaderFromFile(fragfile, &frag_shader);

	// compile and link the programs
	h_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	h_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(h_vertex_shader, 1, (const GLchar **) &vertex_shader, NULL);
	glShaderSource(h_frag_shader, 1, (const GLchar **) &frag_shader, NULL);
	glCompileShader(h_vertex_shader);
	glGetShaderiv(h_vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
	glCompileShader(h_frag_shader);
	glGetShaderiv(h_frag_shader, GL_COMPILE_STATUS, &frag_compiled);
	printInfoLog(h_vertex_shader);
	printInfoLog(h_frag_shader);
	if (!vertex_compiled)
	{
		printf("Error: could not compile vertex shader program %s\n", vertexfile);
		return false;
	}
	if (!frag_compiled)
	{
		printf("Error: could not compile fragment shader program %s\n", fragfile);
		return false;
	}
	h_program = glCreateProgram();
	glAttachShader(h_program, h_vertex_shader);
	glAttachShader(h_program, h_frag_shader);
	glLinkProgram(h_program);
	glGetProgramiv(h_program, GL_LINK_STATUS, &prog_linked);
	printInfoLog(h_program);
	if (!prog_linked)
	{
		printf("Error: could not link shader programs\n");
		return false;
	}

	return true;
}