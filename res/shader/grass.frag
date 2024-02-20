#version 410 core

in vec2 frag_uv;
in float frag_height;
in vec3 vColor;

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

    // Specular
//    float specularStrength = 0.5f;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
//    vec3 specular = specularStrength * spec * lightColor.xyz;

    vec3 result = (ambient + diffuse) * vColor;
    FragColor = vec4(result, 1.0);
}