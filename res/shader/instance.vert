#version 410 core

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in int texid;

out vec2 vTexcoord;
flat out int vTexid;

void main() {
    vec3 worldVertex = position + vertex;

    vTexcoord = texcoord;
    vTexid = texid;

    gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}