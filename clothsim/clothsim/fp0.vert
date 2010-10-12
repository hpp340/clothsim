//
// persptex0.vert
//
// constant reconstruction

uniform mat4 uProjMatrix;		// camera projection matrix
uniform mat4 uModelViewMatrix[27];
uniform mat4 uNormalMatrix[27];

attribute vec3 vColor;

uniform int vMVIndex;

attribute vec2 vTexCoord;		// texture coordinate

varying vec2  pTexCoord;		// texture coordinates
varying vec3 pColor;

varying vec4 pPosition;
varying vec3 pNormal;

int index;

void main()
{
  pTexCoord = vTexCoord;
  pColor = vColor;
 
  vec4 normal = vec4(gl_Normal.x, gl_Normal.y, gl_Normal.z, 1.0);
  pNormal = vec3(uNormalMatrix[vMVIndex] * normal);
  
  pPosition = uModelViewMatrix[vMVIndex] * gl_Vertex;
  gl_Position = uProjMatrix * pPosition;
  
}
