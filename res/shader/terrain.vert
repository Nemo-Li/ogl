#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in  vec2 a_uv;

out float Height;
out vec3 Position;
out vec2 v_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = a_pos.y;
    Position = (view * model * vec4(a_pos, 1.0)).xyz;
    v_uv = a_uv;
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}