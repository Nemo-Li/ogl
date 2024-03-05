#version 410 core

layout (location = 0) in vec3 a_pos;

void main() {
    gl_Position  = a_pos;
    gl_PointSize = 10.0;
}
