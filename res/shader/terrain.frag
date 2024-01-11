#version 410 core

uniform sampler2D u_diffuse_texture;
uniform sampler2D terrain_tex_ground_r;

in vec2 v_uv;
in float Height;

out vec4 FragColor;

void main()
{
    vec4 textureColor = vec4(1.0);
    vec4 textureColor1 = texture(u_diffuse_texture, v_uv);
    vec4 textureColor2 = texture(terrain_tex_ground_r, v_uv);

    float blendPer = smoothstep(-16.0, 48.0, Height);
    textureColor = (1.0 - blendPer) * textureColor1 + blendPer * textureColor2;

    FragColor = vec4(textureColor.xyz, 1.0);
}