#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in int texid;
layout(location = 4) in ivec4 BoneIDs;
layout(location = 5) in vec4 Weights;

out vec3 o_position;
out vec3 o_normal;
out vec2 o_texcoord;
flat out int vTexid;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 viewProj;
uniform mat3 normalMatrix;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

    o_position = vec3(model * BoneTransform * vec4(position, 1.0f));
    o_normal   = normalMatrix * (BoneTransform * vec4(normal, 0)).xyz;

    o_texcoord = texcoord.xy;
    vTexid = texid;

    gl_Position = viewProj * model * BoneTransform * vec4(position, 1.0f);
}