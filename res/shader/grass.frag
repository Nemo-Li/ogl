#version 410 core

in vec2 frag_uv;
in float frag_height;
in vec3 vColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}