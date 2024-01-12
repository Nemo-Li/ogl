#version 410 core

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float texid;

out vec2 vTexcoord;
flat out vec3 vColor;
flat out float vTexid;
out vec4 projPosition;
out vec4 viewPosition;

void main() {
    vColor = vec3(a_color * 0.005);
    vTexid = texid;

    vec4 worldVertex = model * vec4(a_pos, 1.0);

    vTexcoord = a_uv;
    viewPosition = view * worldVertex;
    gl_Position = projection * viewPosition;
    projPosition = gl_Position;
}