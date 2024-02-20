#version 410 core

uniform sampler2D u_diffuse_texture;
uniform sampler2D terrain_tex_ground_r;

in vec2 v_uv;
in float Height;

out vec4 FragColor;
uniform float lightStrength;

vec4 lightColor = vec4(1.0);
float ambientStrength = 0.2f;
vec3 normal = vec3(1.0f, 0.0f, 0.0f);

void main()
{
    // Ambient
    vec3 ambient = ambientStrength * lightColor.xyz;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(vec3(1.0, -1.0, 0.0));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightStrength * lightColor.xyz;

    vec4 textureColor = vec4(1.0);
    vec4 textureColor1 = texture(u_diffuse_texture, v_uv);
    vec4 textureColor2 = texture(terrain_tex_ground_r, v_uv);

    float blendPer = smoothstep(-16.0, 48.0, Height);
    textureColor = (1.0 - blendPer) * textureColor1 + blendPer * textureColor2;

    vec3 result = (ambient + diffuse) * textureColor.xyz;

    FragColor = vec4(result, 1.0);
}