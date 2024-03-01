#version 410 core

#define MAX_MAT 16

const uint MAX_BONE_TEX = 50;

mat3x4 GetBoneTex(sampler2D bone, float boneid, vec4 size, float curr) {
    vec2 t0 = vec2((boneid * 3.0 + 0.0) * size.x, curr) + size.zw;
    vec2 t1 = vec2((boneid * 3.0 + 1.0) * size.x, curr) + size.zw;
    vec2 t2 = vec2((boneid * 3.0 + 2.0) * size.x, curr) + size.zw;

    vec4 f0 = texture(bone, t0);
    vec4 f1 = texture(bone, t1);
    vec4 f2 = texture(bone, t2);

    return mat3x4(f0, f1, f2);
}

struct Material {
    vec4 texids;
    vec4 params;
};

mat4 convertMat(mat3x4 srcMat) {
    mat4x3 transMat = transpose(srcMat);
    return mat4(transMat[0], 0.0,
    transMat[1], 0.0,
    transMat[2], 0.0,
    transMat[3], 1.0);
}

layout(binding = 1, std140) uniform InMaterial {
    Material inMaterials[MAX_MAT];
};

uniform mat4 viewProjectMatrix;
uniform BindlessSampler2D boneTex[MAX_BONE_TEX];

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec4 boneids;
layout (location = 5) in vec4 weights;
layout (location = 6) in mat4 modelMatrix;

out vec2 vTexcoord;
flat out ivec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;

void main() {
    sampler2D bone = boneTex[int(floor(modelMatrix[3].x))];
    vec2 boneSize = vec2(textureSize(bone, 0));
    vec2 invSize = 1.0 / boneSize;
    vec4 size = vec4(invSize, vec2(0.25) * invSize);
    float curr = floor(modelMatrix[3].y) * size.y;

    mat3x4 m0 = GetBoneTex(bone, boneids.x, size, curr);
    mat3x4 m1 = GetBoneTex(bone, boneids.y, size, curr);
    mat3x4 m2 = GetBoneTex(bone, boneids.z, size, curr);
    mat3x4 m3 = GetBoneTex(bone, boneids.w, size, curr);

    mat4 boneMat  = convertMat(m0) * weights.x;
    boneMat += convertMat(m1) * weights.y;
    boneMat += convertMat(m2) * weights.z;
    boneMat += convertMat(m3) * weights.w;

    vec4 position = boneMat * vec4(vertex, 1.0);
    mat4 worldMatrix = convertMat(mat3x4(modelMatrix[0], modelMatrix[1], modelMatrix[2]));

    float insMatid = modelMatrix[3].w;
    int mid = (insMatid > -0.001) ? int(insMatid) : int(texcoord.z);
    Material material = inMaterials[mid];

    mat3 matRot = mat3(worldMatrix);
    mat3 normalMat = matRot * mat3(boneMat);
    vNormal = normalMat * normal;
    vTBN = normalMat * GetTBN(normal, tangent);
    vTexcoord = texcoord.xy;
    vTexid = ivec4(material.texids);
    vColor = material.params.rgb;

    vec4 modelPosition = worldMatrix * position;
    gl_Position = viewProjectMatrix * modelPosition;
}
