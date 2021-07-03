#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in ivec4 BoneID1;
layout (location = 6) in vec4 Weight1;

out vec3 o_position;
out vec3 o_normal;
out vec2 o_texcoord;

const int MAX_BONES = 100;

uniform mat4 world;
uniform mat4 viewProj;
uniform mat3 normalMatrix;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

    BoneTransform     += gBones[BoneID1[0]] * Weight1[0];
    BoneTransform     += gBones[BoneID1[1]] * Weight1[1];
    BoneTransform     += gBones[BoneID1[2]] * Weight1[2];
    BoneTransform     += gBones[BoneID1[3]] * Weight1[3];

    o_position = vec3(world * BoneTransform * vec4(position, 1.0f));
    o_normal   = normalMatrix * (BoneTransform * vec4(normal, 0)).xyz;
    o_texcoord = texcoord.xy;

    gl_Position = viewProj * world * vec4(position, 1.0f);
}