#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D imageTexture;
uniform sampler2D kernelTexture;
uniform vec2 offsetImage;
uniform vec2 offsetKernel; 
uniform float gamma;

vec3  gamma_correction(vec3 color, float coeff){
	return vec3(pow(color.r, coeff), pow(color.g, coeff), pow(color.b, coeff));
} 

void main()
{
	vec3 color = vec3(0.);
	//vec3 colorGammaCorr;
	int ncols = int(1.f/offsetKernel.x);
	int nrows = int(1.f/offsetKernel.y);
	
	int xMid = ncols/2, yMid = nrows/2, x, y;
	vec2 offsetCurrentImage, offsetCurrentKernel, offsetCurrent;
	vec2 center = vec2(0.5, 0.5);
	
	for(y=0; y<nrows; ++y)
	{
		for(x=0; x<ncols; ++x)
		{
			offsetCurrent = vec2(float(x-xMid), float(y-yMid));
			offsetCurrentImage = offsetImage * offsetCurrent;
			offsetCurrentKernel = offsetKernel * offsetCurrent;
			
			color+=gamma_correction(texture(imageTexture, TexCoord+offsetCurrentImage).rgb, gamma)
			* (texture(kernelTexture, center+offsetCurrentKernel).r);
			
			//color += gamma_correction(texture(Texture, TexCoord+offsetCurrent).rgb, gamma) * kernel[y*ncols+x];
		}
	}
	FragColor = vec4(gamma_correction(color, 1.0/gamma), texture(imageTexture, TexCoord).a);
}
