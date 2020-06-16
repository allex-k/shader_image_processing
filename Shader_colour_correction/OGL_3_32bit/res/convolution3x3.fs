#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D Texture;

uniform vec2 offset; 
uniform float[9] coeffVector;
uniform float gamma;

vec3  gamma_correction(vec3 color, float coeff){
	return vec3(pow(color.r, coeff), pow(color.g, coeff), pow(color.b, coeff));
} 

void main()
{
	vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y), // top-left
        vec2( 0.0f,    offset.y), // top-center
        vec2( offset.x,  offset.y), // top-right
        vec2(-offset.x,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset.x,  0.0f),   // center-right
        vec2(-offset.x, -offset.y), // bottom-left
        vec2( 0.0f,   -offset.y), // bottom-center
        vec2( offset.x, -offset.y)  // bottom-right    
    );

	vec3 color = vec3(0.);
	
	for(int i=0; i<9; ++i){
		color += gamma_correction(texture(Texture, TexCoord+offsets[i]).rgb, gamma)*coeffVector[i];
	}

	FragColor = vec4(gamma_correction(color, 1.0/gamma), texture(Texture, TexCoord).a);
}
