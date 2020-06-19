#version 330 core

//optimized blur, verticall and horizontal passages separately
//requires two shader calls for horizontal, for vertical blur
const int MAX_SIZE  = 512;

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D Texture;

uniform int SIZE;
uniform vec2 direction; //The direction of blur taking into account the aspect ratio of image
//step in noramalized coords
//the direction should be (1/imageWidth, 0.0) for horizontal glur, (0.0, 1/imageHeight) for vertical
uniform float[MAX_SIZE] coeffVector;
uniform float gamma;

vec3  gamma_correction(vec3 color, float coeff){
	return vec3(pow(color.r, coeff), pow(color.g, coeff), pow(color.b, coeff));
} 

void main(){
	//float gamma = 2.0;
    FragColor = texture(Texture, TexCoord);
	vec3 color = vec3(0.);
	vec2 newTexCoord;

	//int imin = -SIZE/2, imax = SIZE/2, i;
	
	int imin = -SIZE/2, imax = SIZE/2, i;
	if(SIZE % 2 == 0) --imax;
	
	for(i=imin; i<=imax; i++){
			newTexCoord = vec2(TexCoord+direction*i);
			color += gamma_correction(texture(Texture, newTexCoord).rgb, gamma)*coeffVector[i-imin];}
			
	FragColor = vec4(gamma_correction(color, 1.0/gamma), FragColor.a);
	}
