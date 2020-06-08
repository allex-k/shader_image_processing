#version 330 core

//примітивний блюр матриця коефіціентів квадратна
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D Texture;

uniform int SIZE;
uniform vec2 d; //d-розмір одного пікселя по осях х, y відповідно

const int MAX_SIZE  = 32;
const int MAX_SIZE_2  = MAX_SIZE*MAX_SIZE;

float[MAX_SIZE_2] coeffMatrix;
//const float coeffMatrix[MAX_SIZE_2]  = float[MAX_SIZE_2](1.0);



void main(){
	
    FragColor = texture(Texture, TexCoord);
	vec3 color = vec3(0.);
	vec2 newTexCoord;

	int imin = -SIZE/2, imax = SIZE/2, i, j;
	for(i=imin; i<=imax; i++)
		for(j=imin; j<=imax; j++){
			newTexCoord = vec2(TexCoord+vec2(i,j)*d);
			color += texture(Texture, newTexCoord).rgb*coeffMatrix[j*SIZE+i];}
	color/=(SIZE*SIZE);

	FragColor = vec4(color, FragColor.a);
	}

	

