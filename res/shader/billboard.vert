#version 410 core

uniform mat4 viewProjectMatrix;
uniform vec3 viewRight;
uniform mat4 model;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in int texid;

out vec2 vTexcoord;
flat out int vTexid;

#define TOP_VEC vec3(0.0, 1.0, 0.0)

void main() {
    vec2 size = vertex.xy;
    vec3 right = size.x * viewRight;
    vec3 top = size.y * TOP_VEC;
    vec3 worldVertex = position + right + top;

    vTexcoord = texcoord;
    vTexid = texid;

    gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}