// Arkadiusz Gabrys qe83mepi
// Agnieszka Zacher by57zeja

#version 120

varying vec3 N; 
varying vec4 V;
varying vec2 TexCoord;

uniform vec4 LightSource;  // updated each draw call 
uniform vec4 Color;  // updated each draw call

uniform sampler2D Texture;
 
void main() {

	// TODO: Implement phong shading
	// textures affect diffuse lighting only
	
	// only water should produce specular hightlights

    vec3 L = normalize((LightSource - V).xyz);
    vec3 E = normalize(V.xyz);
    vec3 R = reflect(L, N);

    // spec
    float s = 3;
    float dotPr = dot(E, R);
    if (dotPr < 0.0)
    {
        dotPr = 0.0;
    }

    vec4 spec = vec4(1.0) * pow(dotPr, s);
    vec4 texColor = texture2D(Texture, gl_TexCoord[0].st);

    if (texColor.z >= 0.3 && texColor.x <= 0.5 && texColor.y <= 0.5)
    {
        texColor += 0.5 * spec;
    }

    gl_FragColor =  texColor;
}
