uniform float Time;  // updated each frame by the application

uniform mat4 MVP;  // updated each draw call
uniform mat4 MV;  // updated each draw call
uniform mat3 MormalMatrix;  // updated each draw call

varying float height; // handed to the fragment shader
varying vec3 v_normal;
varying vec3 v_viewingVec;


void main()
{
	/**
		HINT: You can use your own sun-shaders from previous tasks!
	*/
 
 
 
    gl_Position = MVP * gl_Vertex;
 
}
