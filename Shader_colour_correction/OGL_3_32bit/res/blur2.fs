#version 330 core

//������������ ���� ������ �� ��������, �� ���������� ������
//������� ��� ������� ������� ��� ���������������, ��� ������������� ��������
//������� FrameBuffer
const int MAX_SIZE  = 512;

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D Texture;

uniform int SIZE;
uniform vec2 direction; //�������� �������� � ����������� ������������ �����
//�������� ���� � ��������� = 0, (�������� ��� ������������ ��� ��������������)
uniform float[MAX_SIZE] coeffVector;





void main(){
    FragColor = texture(Texture, TexCoord);
	vec3 color = vec3(0.);
	vec2 newTexCoord;

	int imin = -SIZE/2, imax = SIZE/2, i;
	for(i=imin; i<=imax; i++){
			newTexCoord = vec2(TexCoord+direction*i);
			color += texture(Texture, newTexCoord).rgb*coeffVector[i];}
	color*=2.0;

	FragColor = vec4(color, FragColor.a);
	}
