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

    // phong
    int s = 3;
    vec4 diff, spec, ambient;
    float dotPr = 0.0;
    
    // diff
    dotPr = dot(N, L);
    if (dotPr < 0.0)
    {
        dotPr = 0.0;
    }
    diff = Color * dotPr;

    // spec
    dotPr = dot(E, R);
    if (dotPr < 0.0)
    {
        dotPr = 0.0;
    }

    spec = vec4(1.0) * pow(dotPr, s);

    // ambient
    ambient = Color * 0.2;

    //gl_FragColor = ambient + diff + spec;
    gl_FragColor = texture2D(Texture, gl_TexCoord[0].st);
}
