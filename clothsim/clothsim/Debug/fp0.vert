//
// persptex0.vert
//
// constant reconstruction

uniform mat4 uProjMatrix;		// camera projection matrix
uniform mat4 uModelViewMatrix[27];
uniform mat4 uNormalMatrix[27];

attribute vec3 vColor;

attribute float vMVIndex;

attribute vec2 vTexCoord;		// texture coordinate

varying vec2  pTexCoord;		// texture coordinates
varying vec3 pColor;

attribute float v_drawcloth;
varying float p_drawcloth;

varying vec4 pPosition;
varying vec3 pNormal;

void main()
{
  pTexCoord = vTexCoord;
  pColor = vColor;
  p_drawcloth = v_drawcloth;
 
  vec4 normal = vec4(gl_Normal.x, gl_Normal.y, gl_Normal.z, 1.0);
  pNormal = vec3(uNormalMatrix[int(vMVIndex)] * normal);
  
  pPosition = uModelViewMatrix[int(vMVIndex)] * gl_Vertex;
  gl_Position = uProjMatrix * pPosition;
  
}
