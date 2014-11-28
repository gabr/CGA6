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

    // ambient from texture
    vec4 ambient = texture2D(Texture, gl_TexCoord[0].st);

    // diff
    float dotPr = dot(N, L);
    if (dotPr < 0.0)
    {
        dotPr = 0.0;
    }
    vec4 diff = ambient * dotPr;

    // spec
    float s = 3;
    dotPr = dot(E, R);
    if (dotPr < 0.0)
    {
        dotPr = 0.0;
    }

    vec4 spec = vec4(1.0) * pow(dotPr, s);

    if (Color == vec4(0.0)) // background
    {
        gl_FragColor = ambient;
    }
    else if (Color == vec4(1.0)) // planet
    {
        vec4 color = 0.2 * ambient + diff;

        if (color.z >= 0.2 && color.x <= 0.4 && color.y <= 0.4)
        {
            color += 0.5 * spec;
        }

        gl_FragColor = color;
    }
    else // rings
    {
        gl_FragColor = Color;
    }
}
