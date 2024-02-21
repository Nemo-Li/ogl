#version 410 core

uniform sampler2DArray textureArray;
uniform float lightStrength;
uniform vec3 lightDir;

in vec2 vTexcoord;
flat in vec3 vColor;
flat in int vTexid;
in vec4 projPosition;
in vec4 viewPosition;
in vec3 vNormal;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;
layout (location = 2) out vec4 FragNormal;

vec4 lightColor = vec4(1.0);
float ambientStrength = 0.2f;

void main() {
    vec4 textureColor = vec4(1.0);
    vec3 texcoord = vec3(vTexcoord, vTexid);
    if (texcoord.p >= 0.0) {
        textureColor = texture(textureArray, texcoord);
        if (textureColor.a < 0.3)
        discard;
    }

    // Ambient
    vec3 ambient = ambientStrength * lightColor.xyz;

    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 norm_lightDir = normalize(lightDir);
    float diff = max(dot(norm, norm_lightDir), 0.0);
    vec3 diffuse = diff * lightStrength * lightColor.xyz;

    vec3 result = (ambient + diffuse) * textureColor.xyz;
    FragColor = vec4(result, 1.0);
}