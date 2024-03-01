//
// Created by Nemo li on 2024/2/27.
//

#include "ass_loader.h"
#include "assimp/postprocess.h"
#include "ass_animation.h"
#include "framework/application.h"
#include "helpers/debug.h"
#include "helpers/material_manager.h"
#include "renderer/texture3d.h"
#include <glm/gtx/string_cast.hpp>

AssLoader::AssLoader(const char *path) {
    assAnimation = new AssAnimation();
    texture3D = new Texture3D;
    boneMap.clear();
    boneInfos.clear();
    materialMap.clear();
    importer = new Assimp::Importer();
    path_ = std::string(Application::data_path()) + path;
    scene = importer->ReadFile(path_,
                              aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
                              aiProcess_JoinIdenticalVertices);

    loadModel();
    for (int ai = 0; ai < scene->mNumAnimations; ++ai) {
        aiAnimation *ai_animation = scene->mAnimations[ai];
        AnimFrame *animation = new AnimFrame(ai_animation->mName.C_Str());
        printf("assimp anim name:%s\n", animation->getName().data());
        assAnimation->datasToExport.push_back(animation);
        prepareFrameData(ai, ai_animation, animation);
    }
}

AssLoader::~AssLoader() {
    delete[] entries;
    delete texture3D;

    boneMap.clear();
    for (unsigned int i = 0; i < boneInfos.size(); i++)
        free(boneInfos[i]);
    boneInfos.clear();

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        channelMaps[i].clear();
    delete[] channelMaps;

    importer->FreeScene();
    materialMap.clear();
}

void AssLoader::loadModel() {
    loadMaterials();
    int meshCount = scene->mNumMeshes;
//    DEBUG_LOG_INFO("meshCount {}", meshCount);
    entries = new Entry[meshCount];
    int base = 0;
    for (int i = 0; i < meshCount; i++) {
        aiMesh *mesh = scene->mMeshes[i];
        entries[i].mesh = mesh;
        entries[i].materialIndex = mesh->mMaterialIndex;
        entries[i].baseVertex = base;
        base += mesh->mNumVertices;
        loadMeshes(&entries[i]);
    }

    assAnimation->aBoneids.resize(assAnimation->vertCount);
    assAnimation->aWeights.resize(assAnimation->vertCount);
    for (int i = 0; i < assAnimation->vertCount; i++) {
        assAnimation->aBoneids[i] = glm::uvec4(0, 0, 0, 0);
        assAnimation->aWeights[i] = glm::vec4(0, 0, 0, 0);
    }
    for (int i = 0; i < meshCount; i++) {
        aiMesh *mesh = scene->mMeshes[i];
        loadBones(mesh, i);
    }

    rootToModelMat = scene->mRootNode->mTransformation;
    rootToModelMat = rootToModelMat.Inverse();

    channelMaps = new std::map<std::string, aiNodeAnim *>[scene->mNumAnimations];
    initChannels();

    std::string l_file = path_;
    int index = l_file.find_last_of('/');
    std::string path = l_file.substr(0, index);
    printf("ass model path: %s\n", path.c_str());
    texture3D->initTexture3D(path.append("/textures"));

    assAnimation->texture3D = texture3D;
}

void AssLoader::loadMaterials() {
    unsigned int matCount = scene->mNumMaterials;
    for (int i = 0; i < matCount; ++i) {
        aiMaterial *mat = scene->mMaterials[i];
        aiString path, name;
        aiColor4D ambient(0, 0, 0, 1), diffuse(0, 0, 0, 1), specular(0, 0, 0, 1);
        if (mat->Get(AI_MATKEY_NAME, name) != AI_SUCCESS) name = "animation_mat";
        printf("material name %s\n", name.C_Str());
        auto *mtl = new Material();
        mtl->name_ = std::string(name.C_Str());
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            mtl->tex1 = assAnimation->convertTexPath(path.data);
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
            mtl->tex2 = assAnimation->convertTexPath(path.data);
        if (mat->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
            mtl->tex3 = assAnimation->convertTexPath(path.data);
        if (mat->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
            mtl->tex4 = assAnimation->convertTexPath(path.data);
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambient);
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
        if (ambient.r < 0.2) ambient.r = 0.2;
        mtl->ambient.x = ambient.r;
        mtl->ambient.y = ambient.g;
        mtl->ambient.z = ambient.b;
        mtl->diffuse.x = diffuse.r;
        mtl->diffuse.y = diffuse.g;
        mtl->diffuse.z = diffuse.b;
        mtl->specular.x = specular.r;
        mtl->specular.y = specular.g;
        mtl->specular.z = specular.b;
//        materialMap[i] = MaterialManager::instance->add(mtl);
        if (texture3D->findImage(mtl->tex1.c_str()) < 0)
            texture3D->addImage(mtl->tex1.c_str());
        materialMap[i] = texture3D->findImage(mtl->tex1.c_str());
    }
}

void AssLoader::loadMeshes(Entry *entry) {
    aiMesh *aMesh = entry->mesh;
    int aVertCount = aMesh->mNumVertices;
    assAnimation->vertCount += aVertCount;
    for (int i = 0; i < aVertCount; i++) {
        aiVector3D *vertex = &aMesh->mVertices[i];
        aiVector3D *normal = &aMesh->mNormals[i];
        assAnimation->aVertices.emplace_back(vertex->x, vertex->y, vertex->z);
//        DEBUG_LOG_INFO("vertex: {} {} {}", vertex->x, vertex->y, vertex->z);
        assAnimation->aNormals.emplace_back(normal->x, normal->y, normal->z);
        if (aMesh->HasTangentsAndBitangents()) {
            aiVector3D *tangent = &aMesh->mTangents[i];
            assAnimation->aTangents.emplace_back(tangent->x, tangent->y, tangent->z);
        }
        if (aMesh->HasTextureCoords(0)) {
            aiVector3D *texcoord = &aMesh->mTextureCoords[0][i];
            assAnimation->aTexcoords.emplace_back(texcoord->x, texcoord->y);
        } else
            assAnimation->aTexcoords.emplace_back(0, 0);

        unsigned int mid = materialMap[entry->materialIndex];
//        Material *mat = MaterialManager::instance->find(mid);
//        if (!mat) {
//            mid = 0;
//            mat = MaterialManager::instance->find(mid);
//        }
        assAnimation->aMids.push_back(mid);
//        assAnimation->aTextures.push_back(mat);
//        assAnimation->aAmbients.push_back(mat->ambient);
//        assAnimation->aDiffuses.push_back(mat->diffuse);
//        assAnimation->aSpeculars.push_back(mat->specular);
    }
    int aFaceCount = aMesh->mNumFaces;
    assAnimation->faceCount += aFaceCount;
    for (int i = 0; i < aFaceCount; i++) {
        aiFace face = aMesh->mFaces[i];
        unsigned int index1 = entry->baseVertex + face.mIndices[0];
        unsigned int index2 = entry->baseVertex + face.mIndices[1];
        unsigned int index3 = entry->baseVertex + face.mIndices[2];
//        DEBUG_LOG_INFO("index: {} {} {}", index1, index2, index3);

        assAnimation->aIndices.push_back(index1);
        assAnimation->aIndices.push_back(index2);
        assAnimation->aIndices.push_back(index3);
    }
//    DEBUG_LOG_INFO("vertCount {} faceCount {}", assAnimation->vertCount, assAnimation->faceCount);
}

void AssLoader::loadBones(aiMesh *mesh, int meshIndex) {
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone *bone = mesh->mBones[i];
        std::string boneName = bone->mName.data;
//        DEBUG_LOG_INFO("boneName {}", boneName);
        int boneIndex = 0;
        if (boneMap.find(boneName) == boneMap.end()) {
            boneIndex = assAnimation->boneCount;
            assAnimation->boneCount++;

            BoneInfo *boneInfo = (BoneInfo *) malloc(sizeof(BoneInfo));
            boneInfo->offset = bone->mOffsetMatrix;
            boneInfos.push_back(boneInfo);
            boneMap[boneName] = boneIndex;
        } else
            boneIndex = boneMap[boneName];

        for (unsigned int j = 0; j < bone->mNumWeights; j++) {
            aiVertexWeight boneWeight = bone->mWeights[j];
            int vertexId = entries[meshIndex].baseVertex + boneWeight.mVertexId;
            float weight = boneWeight.mWeight;
            pushWeightToVertex(vertexId, boneIndex, weight);
        }
    }
}

void AssLoader::pushWeightToVertex(int vertexId, int boneId, float weight) {
    if (assAnimation->aWeights[vertexId].x == 0) {
        assAnimation->aWeights[vertexId].x = weight;
        assAnimation->aBoneids[vertexId].x = boneId;
    } else if (assAnimation->aWeights[vertexId].y == 0) {
        assAnimation->aWeights[vertexId].y = weight;
        assAnimation->aBoneids[vertexId].y = boneId;
    } else if (assAnimation->aWeights[vertexId].z == 0) {
        assAnimation->aWeights[vertexId].z = weight;
        assAnimation->aBoneids[vertexId].z = boneId;
    } else if (assAnimation->aWeights[vertexId].w == 0) {
        assAnimation->aWeights[vertexId].w = weight;
        assAnimation->aBoneids[vertexId].w = boneId;
    }
}

void AssLoader::initChannels() {
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation *animation = scene->mAnimations[i];
        std::map<std::string, aiNodeAnim *> channelMap;
        channelMap.clear();
        for (unsigned int j = 0; j < animation->mNumChannels; j++) {
            aiNodeAnim *nodeAnim = animation->mChannels[j];
            std::string boneName(nodeAnim->mNodeName.data);
            channelMap[boneName] = nodeAnim;
        }
        channelMaps[i] = channelMap;
    }
}

int AssLoader::findPositionIndex(aiNodeAnim *anim, float animTime) {
    for (unsigned int i = 0; i < anim->mNumPositionKeys - 1; i++) {
        if (animTime < anim->mPositionKeys[i + 1].mTime)
            return i;
    }
    return 0;
}

int AssLoader::findRotationIndex(aiNodeAnim *anim, float animTime) {
    for (unsigned int i = 0; i < anim->mNumRotationKeys - 1; i++) {
        if (animTime < anim->mRotationKeys[i + 1].mTime)
            return i;
    }
    return 0;
}

int AssLoader::findScaleIndex(aiNodeAnim *anim, float animTime) {
    for (unsigned int i = 0; i < anim->mNumScalingKeys - 1; i++) {
        if (animTime < anim->mScalingKeys[i + 1].mTime)
            return i;
    }
    return 0;
}

void AssLoader::calcPosition(aiNodeAnim *anim, float animTime, aiVector3D &position) {
    if (anim->mNumPositionKeys == 1) {
        position = anim->mPositionKeys[0].mValue;
        return;
    }

    int startId = findPositionIndex(anim, animTime);
    int endId = startId + 1;
    aiVectorKey startKey = anim->mPositionKeys[startId];
    aiVectorKey endKey = anim->mPositionKeys[endId];
    double dKeyTime = (endKey.mTime - startKey.mTime);
    double dTime = (double) animTime - startKey.mTime;
    double factor = dTime / dKeyTime;
    aiVector3D startPosition = startKey.mValue;
    aiVector3D endPosition = endKey.mValue;
    aiVector3D dPosition = endPosition - startPosition;
    aiVector3D sPosition(factor * dPosition.x, factor * dPosition.y, factor * dPosition.z);
    position = startPosition + sPosition;
}

void AssLoader::calcRotation(aiNodeAnim *anim, float animTime, aiQuaternion &rotation) {
    if (anim->mNumRotationKeys == 1) {
        rotation = anim->mRotationKeys[0].mValue.Normalize();
        return;
    }
    int startId = findRotationIndex(anim, animTime);
    int endId = startId + 1;
    aiQuatKey startKey = anim->mRotationKeys[startId];
    aiQuatKey endKey = anim->mRotationKeys[endId];
    double dKeyTime = endKey.mTime - startKey.mTime;
    double dTime = (double) animTime - startKey.mTime;
    double factor = dTime / dKeyTime;
    aiQuaternion startRotation = startKey.mValue;
    aiQuaternion endRotation = endKey.mValue;
    aiQuaternion::Interpolate(rotation, startRotation, endRotation, factor);
    rotation = rotation.Normalize();
}

void AssLoader::calcScale(aiNodeAnim *anim, float animTime, aiVector3D &scale) {
    if (anim->mNumScalingKeys == 1) {
        scale = anim->mScalingKeys[0].mValue;
        return;
    }

    int startId = findScaleIndex(anim, animTime);
    int endId = startId + 1;
    aiVectorKey startKey = anim->mScalingKeys[startId];
    aiVectorKey endKey = anim->mScalingKeys[endId];
    double dKeyTime = endKey.mTime - startKey.mTime;
    double dTime = (double) animTime - startKey.mTime;
    double factor = dTime / dKeyTime;
    aiVector3D startScale = startKey.mValue;
    aiVector3D endScale = endKey.mValue;
    aiVector3D dScale = endScale - startScale;
    aiVector3D sScale(factor * dScale.x, factor * dScale.y, factor * dScale.z);
    scale = startScale + sScale;
}

aiNodeAnim *AssLoader::findNodeAnim(int animIndex, std::string boneName) {
    std::map<std::string, aiNodeAnim *> channelMap = channelMaps[animIndex];
    if (channelMap.find(boneName) != channelMap.end()) {
        aiNodeAnim *nodeAnim = channelMap[boneName];
        return nodeAnim;
    }
    return nullptr;
}

void AssLoader::readNode(int animIndex, float animTime, aiNode *node, const aiMatrix4x4 &parentTransform) {
    std::string boneName(node->mName.data);
    aiMatrix4x4 boneTransform = node->mTransformation;
    aiNodeAnim *boneAnim = findNodeAnim(animIndex, boneName);
    if (boneAnim) {
        aiVector3D scale;
        calcScale(boneAnim, animTime, scale);
        aiMatrix4x4 scaleMat;
        aiMatrix4x4::Scaling(scale, scaleMat);

        aiQuaternion rotation;
        calcRotation(boneAnim, animTime, rotation);
        aiMatrix4x4 rotateMat(rotation.GetMatrix());

        aiVector3D position;
        calcPosition(boneAnim, animTime, position);
        aiMatrix4x4 translateMat;
        aiMatrix4x4::Translation(position, translateMat);

        boneTransform = translateMat * rotateMat * scaleMat;
    }

    aiMatrix4x4 currentBoneTransform = parentTransform * boneTransform;
    if (boneMap.find(boneName) != boneMap.end()) {
        int boneIndex = boneMap[boneName];
        BoneInfo *boneInfo = boneInfos[boneIndex];
        aiMatrix4x4 aTransform = rootToModelMat * currentBoneTransform * boneInfo->offset;
        boneInfo->transformation = aTransform;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        aiNode *childNode = node->mChildren[i];
        readNode(animIndex, animTime, childNode, currentBoneTransform);
    }
}

void AssLoader::prepareFrameData(int animIndex, aiAnimation *asAnimation, AnimFrame *animation) {
    aiMatrix4x4 mat;
    glm::mat4 mat4;

    for (float tick = 0.0f; tick < asAnimation->mDuration; tick += 0.01f) {
        Frame *frame = new Frame(assAnimation->boneCount);
        int currIndex = 0;
        readNode(animIndex, tick, scene->mRootNode, mat);
        for (int bi = 0; bi < assAnimation->boneCount; bi++) {
            mat4[0][0] = boneInfos[bi]->transformation.a1;
            mat4[0][1] = boneInfos[bi]->transformation.b1;
            mat4[0][2] = boneInfos[bi]->transformation.c1;
            mat4[0][3] = boneInfos[bi]->transformation.d1;
            mat4[1][0] = boneInfos[bi]->transformation.a2;
            mat4[1][1] = boneInfos[bi]->transformation.b2;
            mat4[1][2] = boneInfos[bi]->transformation.c2;
            mat4[1][3] = boneInfos[bi]->transformation.d2;
            mat4[2][0] = boneInfos[bi]->transformation.a3;
            mat4[2][1] = boneInfos[bi]->transformation.b3;
            mat4[2][2] = boneInfos[bi]->transformation.c3;
            mat4[2][3] = boneInfos[bi]->transformation.d3;
            mat4[3][0] = boneInfos[bi]->transformation.a4;
            mat4[3][1] = boneInfos[bi]->transformation.b4;
            mat4[3][2] = boneInfos[bi]->transformation.c4;
            mat4[3][3] = boneInfos[bi]->transformation.d4;
            mat4 = glm::transpose(mat4);
            for (int m = 0; m < 16; m++) {
                int i = m / 4;
                int j = m % 4;
//                DEBUG_LOG_INFO("{} {}", i, j);
                frame->data[currIndex++] = mat4[i][j];
            }
//            DEBUG_LOG_INFO("{}", glm::to_string(mat4));
//            DEBUG_LOG_INFO("{}", glm::to_string(mat4[3]));
        }
        animation->frames.push_back(frame);
    }
    animation->setDuration(asAnimation->mDuration);
    animation->setTicksPerSecond(asAnimation->mTicksPerSecond);
}