//
// Created by Nemo li on 2024/2/27.
//

#ifndef OGL_ASS_LOADER_H
#define OGL_ASS_LOADER_H

#include <map>
#include <vector>
#include "assimp/mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "model/ass_animation.h"

struct Entry {
    aiMesh *mesh;
    int baseVertex;
    int materialIndex;
};

struct BoneInfo {
    aiMatrix4x4 offset;
    aiMatrix4x4 transformation;
};

class AssLoader {
private:
    AssAnimation *assAnimation;
    Assimp::Importer importer;
    const aiScene *scene;
    Entry *entries;
    std::map<std::string, int> boneMap;
    std::vector<BoneInfo *> boneInfos;
    aiMatrix4x4 rootToModelMat;
    std::map<std::string, aiNodeAnim *> *channelMaps;
    std::map<int, int> materialMap;

private:
    void loadModel();

    void loadMaterials();

    void loadMeshes(Entry *entry);

    void loadBones(aiMesh *mesh, int meshIndex);

    void pushWeightToVertex(int vertexId, int boneId, float weight);

    void initChannels();

    int findPositionIndex(aiNodeAnim *anim, float animTime);

    int findRotationIndex(aiNodeAnim *anim, float animTime);

    int findScaleIndex(aiNodeAnim *anim, float animTime);

    void calcPosition(aiNodeAnim *anim, float animTime, aiVector3D &position);

    void calcRotation(aiNodeAnim *anim, float animTime, aiQuaternion &rotation);

    void calcScale(aiNodeAnim *anim, float animTime, aiVector3D &scale);

    aiNodeAnim *findNodeAnim(int animIndex, std::string boneName);

    void readNode(int animIndex, float animTime, aiNode *node, const aiMatrix4x4 &parentTransform);

    void prepareFrameData(int animIndex, aiAnimation *asAnimation, AnimFrame *animation);

public:
    AssLoader(const char *path);

    virtual ~AssLoader();
};


#endif //OGL_ASS_LOADER_H
