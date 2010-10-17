//
// persptex0.frag
//
// constant reconstruction
//

uniform vec3 uLight;

uniform int uDrawCloth;

uniform float uTw;
uniform float uTh;
uniform sampler2D texUnit0;

varying vec2  pTexCoord;		// texture coordinates

varying vec4 pPosition;
varying vec3 pNormal;
varying vec3 pColor;


void main()
{
   
   if(uDrawCloth == 1)
   {
		vec4 texColor0 = texture2D(texUnit0, pTexCoord);
		vec3 texel;
		texel.x = texColor0.x;
		texel.y = texColor0.y;
		texel.z = texColor0.z;
		vec3 tolight = uLight - vec3(pPosition);
		tolight = normalize(tolight);
		vec3 normal = normalize(pNormal);
		float diffuse = max(0.0, dot(normal, tolight));
		vec3  intensity = texel * diffuse;
		gl_FragColor = vec4(intensity.x, intensity.y, intensity.z, 1.0);   
	}
	else
	{
		vec3 tolight = uLight - vec3(pPosition);
		tolight = normalize(tolight);

		vec3 normal = normalize(pNormal);

		float diffuse = max(0.0, dot(normal, tolight));
		vec3  intensity = pColor * diffuse;

		vec4 tmpCol = vec4(intensity.x, intensity.y, intensity.z, 1.0);
		gl_FragColor = tmpCol;
	}
}
