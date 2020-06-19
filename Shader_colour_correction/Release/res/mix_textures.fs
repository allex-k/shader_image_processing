#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float coeff;

float average(vec3 val){
return (val.r+val.g+val.b)/3.0;}

void main(){
	float coeff2 = average(texture(texture2, TexCoord).rgb);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), coeff2);
}