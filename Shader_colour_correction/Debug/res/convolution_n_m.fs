#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 offset; 
uniform float gamma;
uniform int nrows;
uniform int ncols;

const int MAX_SIZE  = 512;
uniform float[MAX_SIZE] kernel;

vec3  gamma_correction(vec3 color, float coeff){
	return vec3(pow(color.r, coeff), pow(color.g, coeff), pow(color.b, coeff));
} 

void main()
{
	vec3 color = vec3(0.);
	
	int xMid = ncols/2, yMid = nrows/2, x, y;
	vec2 offsetCurrent;
	
	for(y=0; y<nrows; ++y)
	{
		for(x=0; x<ncols; ++x)
		{
			offsetCurrent = offset * vec2(float(x-xMid), float(y-yMid));
			color += gamma_correction(texture(Texture, TexCoord+offsetCurrent).rgb, gamma) * kernel[y*ncols+x];
		}
	}
	FragColor = vec4(gamma_correction(color, 1.0/gamma), texture(Texture, TexCoord).a);
}
