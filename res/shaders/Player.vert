#version 330 core
layout (location = 0) in vec2 aTexCoord;
layout (location = 1) in vec3 aPos;
out vec2 aCoord;
uniform mat4 world;
uniform mat4 viewProj;

void main(){
	gl_Position = viewProj * world * vec4(aPos, 1.0f);
	aCoord = aTexCoord;
}