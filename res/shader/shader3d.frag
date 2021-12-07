#version 330 core
out vec4 FragColor;

uniform vec4 bgColor;

void main()
{
    FragColor = bgColor;
}