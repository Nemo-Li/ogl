#version 410 core

uniform sampler2D u_diffuse_texture;
uniform sampler2D terrain_tex_ground_r;

in vec2 frag_uv;
in float frag_height;
in vec3 vColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}