#version 330 core

uniform sampler2D u_diffuse_texture;

in vec2 v_uv;
in float Height;

out vec4 FragColor;

void main()
{
    // shift and scale the height into a grayscale value
    float h = (Height + 16)/32.0f;
    vec4 tex_color = texture(u_diffuse_texture, v_uv);
    FragColor = vec4(tex_color.xyz, 1.0);
}