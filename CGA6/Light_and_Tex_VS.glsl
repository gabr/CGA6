// Arkadiusz Gabrys qe83mepi
// Agnieszka Zacher by57zeja

#version 120

uniform mat4 MVP;  // updated each draw call
uniform mat4 MV;  // updated each draw call 
uniform mat3 NormalMatrix;  // updated each draw call

uniform vec4 LightSource;  // updated each draw call

varying vec3 N;
varying vec4 V;
varying vec2 TexCoord;

void main() { 

	// TODO:
	// reuse your phong vs code
	// interpolate tex coordinates over the surface using the provided varying

    N = normalize(NormalMatrix * gl_Normal);
	V = MV * gl_Vertex;

	gl_Position = MVP * gl_Vertex;
}