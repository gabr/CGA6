uniform vec4 Color;  // updated each draw call
uniform sampler2D Texture;

varying vec3 N; 
varying vec4 V;
varying vec2 TexCoord;

uniform vec4 LightSource;  // updated each draw call
 

void main() {

	// TODO: Implement phong shading
	// textures affect diffuse lighting only
	
	// only water should produce specular hightlights

	gl_FragColor =  Color; 

}