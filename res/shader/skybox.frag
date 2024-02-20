#version 410 core
out vec4 FragColor;

in vec3 v_uv;

uniform samplerCube skybox;
uniform float lightStrength;

vec4 lightColor = vec4(1.0);
float ambientStrength = 0.2f;

void main()
{
    vec4 skyColor = texture(skybox, v_uv);
    vec3 ambient = ambientStrength * lightColor.xyz;
    vec3 diffuse = 0.9f * lightStrength * lightColor.xyz;
    FragColor = vec4((ambient + diffuse) * skyColor.xyz, 1.0f);
}