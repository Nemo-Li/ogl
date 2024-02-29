#version 410 core

uniform sampler2DArray textureArray;
uniform sampler2D shadowMap;

uniform float lightStrength;
uniform vec3 lightDir;

in vec2 vTexcoord;
flat in vec3 vColor;
flat in int vTexid;
in vec4 projPosition;
in vec4 viewPosition;
in vec3 vNormal;
in vec4 FragPosLightSpace;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;
layout (location = 2) out vec4 FragNormal;

vec4 lightColor = vec4(1.0);
float ambientStrength = 0.2f;

float ShadowCalculation(vec4 fragPosLightSpace){
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth < closestDepth  ? 1.0 : 0.0;

    return shadow;
}

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

    // 计算阴影
    float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 result = (ambient + (1.0 - shadow) * diffuse) * textureColor.xyz;
    FragColor = vec4(result, 1.0);
}