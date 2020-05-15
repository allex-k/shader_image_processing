#version 330 core

in vec2 TexCoord;
out vec4 fragColor;
uniform sampler2D Texture;

uniform int id;
uniform float value;

vec3 saturation(vec3 col, float coeff){
	float bright = (col.r+col.g+col.b)/3.;
	vec3 brightVec = vec3(bright);
	vec3 col2=col.rgb-brightVec;
	return brightVec + col2*coeff;
}

vec3  gamma_correction(vec3 color, float coeff){
	return vec3(pow(color.r, coeff), pow(color.g, coeff), pow(color.b, coeff));
} 

vec3 brightness(vec3 color, float coeff){
	return color.rgb*coeff;  }

vec3 contrast(vec3 color, float d){
	float val1 = 0.5-d/2.0, val2 = 0.5+d/2.0;
	return smoothstep(val1, val2, color.rgb);
}

vec3 color_only(vec3 color){
	float bright = (color.r+color.g+color.b)/3.;
	vec3 brightVec = vec3(bright);

	return 0.5+(color-brightVec);
}

//from stack overflow
// All components are in the range [0…1], including hue.
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
//from stack overflow
// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 hue(vec3 col, float coeff){
	vec3 hsv = rgb2hsv(col);
    hsv[0]+=coeff;
    return hsv2rgb(hsv);
}
vec3 tritanopia_color_blindness(vec3 col){
	return vec3(col.r, col.g, col.g);
}

void main(){
	
    fragColor = texture(Texture, TexCoord);
	vec3 color = fragColor.rgb;

	 switch(id){
		case 0: color = saturation(color, value); break;
		case 1: color = gamma_correction(color, value); break;
		case 2: color = brightness(color, value); break;
		case 3: color = contrast(color, value); break;
		case 4: color = color_only(color); break;
		case 5: color = hue(color, value); break;
		case 6:
			color.r = texture(Texture, TexCoord+vec2(value, 0.)).r;
			color.b = texture(Texture, TexCoord+vec2(-value, 0.)).b;
			break;
		case 7: color = tritanopia_color_blindness(color); break;
		
	}

	//color = saturation(color, 1.5);
	//color = hue(color, -0.08);
	

	fragColor = vec4(color, fragColor.a);
	
}