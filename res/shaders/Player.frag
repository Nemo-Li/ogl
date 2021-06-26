#version 330 core
out vec4 FragColor;
vec3 yuv;
in vec2 aCoord;
uniform sampler2D texture1;
void main(){
	yuv = texture(texture1, aCoord).rgb;
	FragColor = vec4(yuv,1.0);
}