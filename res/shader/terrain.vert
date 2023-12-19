#version 330 core
layout (location = 0) in vec3 a_pos;

out float Height;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = a_pos.y;
    Position = (view * model * vec4(a_pos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}