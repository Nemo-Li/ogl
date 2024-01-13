#version 410 core

uniform sampler2DArray textureArray;

in vec2 vTexcoord;
flat in vec3 vColor;
flat in float vTexid;
in vec4 projPosition;
in vec4 viewPosition;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;
layout (location = 2) out vec4 FragNormal;

void main() {
    vec4 textureColor = vec4(1.0);
    vec3 texcoord = vec3(vTexcoord, vTexid);
    if (texcoord.p >= 0.0) {
        textureColor = texture(textureArray, texcoord);
        if (textureColor.a < 0.1)
        discard;
    }

    float ambientFactor = 0.6; float diffuseFactor = 1.2;
    vec3 ambientColor = vColor.xxx * ambientFactor;
    vec3 diffuseColor = vColor.yyy * diffuseFactor;

    float shadowFactor = 1.0;

    FragColor.rgb = textureColor.rgb; //* (ambientColor + shadowFactor * diffuseColor);
    FragColor.a = 1.0;

    float depth = projPosition.z / projPosition.w;
    FragDepth = vec4(depth, depth, depth, depth);
}