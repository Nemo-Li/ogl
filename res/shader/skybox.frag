#version 410 core
out vec4 FragColor;

in vec3 v_uv;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, v_uv);
}