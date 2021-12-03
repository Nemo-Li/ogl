/** 
 * Code taken from www.learnopengl.com
 * Slightly modified by Tomasz Gałaj 2018
 **/

#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

#include "Mesh.h"
#include "helpers/RootDir.h"
#include <assimp/matrix4x4.h>
#include "math/math_3d.h"

using namespace std;
#define INVALID_MATERIAL 0xFFFFFFFF

class Model {
public:
    /*  Model Data */
    std::vector<Mesh *> meshes;
    std::vector<Texture *> m_Textures;

    // maps a bone name to its index
    map<string, uint> m_BoneMapping;
    map<string, const aiNodeAnim *> m_NodeAnimMapping;

    vector<BoneInfo> m_BoneInfo;
    aiAnimation *pAnimation = nullptr;
    aiNode *rootNode;
    float TicksPerSecond;
    float Duration;
    const aiScene *scene;
    bool hasAnimation;

    struct MeshEntry {
        MeshEntry() {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    vector<MeshEntry> m_Entries;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path) {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw() {
        for (unsigned int i = 0; i < meshes.size(); i++) {
            meshes[i]->Draw();
        }
    }

    uint getBoneNum() {
        return m_NumBones;
    }

    void BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms) {
        if (!hasAnimation) {
            return;
        }
        Matrix4f Identity;
        Identity.InitIdentity();

        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float AnimationTime = fmod(TimeInTicks, Duration);
        ReadNodeHierarchy(AnimationTime, rootNode, Identity);

        Transforms.resize(m_NumBones);

        for (uint i = 0; i < m_NumBones; i++) {
            Transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
    }

private:
    uint m_NumBones = 0;
    Matrix4f m_GlobalInverseTransform;

    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path) {
        // read file via ASSIMP
        // 非常关键的地方，一定不能定义为对象，不然直接被回收，出现内存的一系列问题，很难排查
        // 多谢：https://blog.csdn.net/nice_xp/article/details/94624153?spm=1001.2014.3001.5501
        // 后续看看其它人实现: https://github.com/Alistair401/AssimpViewer/blob/master/Import.cpp
        auto *importer = new Assimp::Importer();
        scene = importer->ReadFile(ROOT_DIR + path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                    aiProcess_CalcTangentSpace);
        rootNode = scene->mRootNode;
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !rootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer->GetErrorString() << std::endl;
            return;
        }
        //根节点的转换矩阵
        m_GlobalInverseTransform = Matrix4f(rootNode->mTransformation);
        m_GlobalInverseTransform.Inverse();

        std::cout << "mNumMeshes--" << scene->mNumMeshes << std::endl;
        //记录所有mesh的
        m_Entries.resize(scene->mNumMeshes);
        m_Textures.resize(scene->mNumMaterials);

        uint NumVertices = 0;
        uint NumIndices = 0;

        std::cout << "m_Entries size--" << m_Entries.size() << std::endl;

        // Count the number of vertices and indices
        for (uint i = 0; i < m_Entries.size(); i++) {
            m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
            m_Entries[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
            m_Entries[i].BaseVertex = NumVertices;
            m_Entries[i].BaseIndex = NumIndices;

            NumVertices += scene->mMeshes[i]->mNumVertices;
            NumIndices += m_Entries[i].NumIndices;
            std::cout << "BaseVertex--" << NumVertices << std::endl;
            std::cout << "BaseIndex--" << NumIndices << std::endl;
        }

        initMaterial(scene);
        initAnimation(scene);
        processNode(rootNode, scene);
    }

    void initAnimation(const aiScene *scene) {
        unsigned int mNumAnimations = scene->mNumAnimations;
        if (mNumAnimations == 0) {
            hasAnimation = false;
            return;
        }
        hasAnimation = true;
        pAnimation = scene->mAnimations[0];
        TicksPerSecond = pAnimation->mTicksPerSecond;
        Duration = pAnimation->mDuration;
        cout << "获取动画时间:" << pAnimation->mDuration << endl;
    }

    void initMaterial(const aiScene *scene) {
        // Initialize the materials
        for (uint i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial *pMaterial = scene->mMaterials[i];
            m_Textures[i] = nullptr;

            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString Path;

                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr,
                                          nullptr) == AI_SUCCESS) {
                    std::string p(Path.data);

                    if (p.substr(0, 2) == ".\\") {
                        p = p.substr(2, p.size() - 2);
                    }
                    p = p.substr(p.find_last_of("\\") + 1, p.length());

                    std::string FullPath = p;
                    std::string Prefix = "res/models/";
                    //const string &basicString = subReplace(FullPath, ".tga", ".png");
                    const string &basicString = subReplace(FullPath, "*", "tsz");
                    std::string finalPath = Prefix.append(basicString).append(".png");
                    auto *pTexture = new Texture(finalPath);
                    m_Textures[i] = pTexture;

                    if (!m_Textures[i]->load()) {
                        printf("Error loading texture '%s'\n", finalPath.c_str());
                        delete m_Textures[i];
                        m_Textures[i] = nullptr;
                    } else {
                        printf("%d - loaded texture '%s'\n", i, finalPath.c_str());
                    }
                }
            }
        }
    }

    std::string subReplace(std::string resource_str, std::string sub_str, std::string new_str) {
        std::string dst_str = resource_str;
        std::string::size_type pos = 0;
        while ((pos = dst_str.find(sub_str)) != std::string::npos)   //替换所有指定子串
        {
            dst_str.replace(pos, sub_str.length(), new_str);
        }
        return dst_str;
    }

    // processes a node in a recursive fashion. Processes each individual pModel located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene) {

        // process each pModel located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        if (hasAnimation) {
            //处理骨骼动画信息
            string NodeName(node->mName.data);
            static const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, NodeName);
            if (pNodeAnim != nullptr) {
                m_NodeAnimMapping[NodeName] = pNodeAnim;
            }
        }

        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void LoadBones(uint MeshIndex, const aiMesh *pMesh, vector<VertexBoneData> &Bones) {
        cout << "当前网格:" << MeshIndex << " 骨骼数量:" << pMesh->mNumBones << endl;
        for (uint i = 0; i < pMesh->mNumBones; i++) {
            uint BoneIndex = 0;
            string BoneName(pMesh->mBones[i]->mName.data);

            if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
                // Allocate an index for a new bone
                BoneIndex = m_NumBones;
                m_NumBones++;
                BoneInfo bi;
                m_BoneInfo.push_back(bi);
                m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
                m_BoneMapping[BoneName] = BoneIndex;
            } else {
                BoneIndex = m_BoneMapping[BoneName];
            }

            for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
                uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
                float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
                if (VertexID >= Bones.size()) {
                    Bones.resize(VertexID + 1);
                }
                Bones[VertexID].AddBoneData(BoneIndex, Weight);
            }
        }
    }

    Mesh *processMesh(aiMesh *mesh, const aiScene *scene) {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<VertexBoneData> bones;

        // Walk through each of the pModel's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // normals
            if (mesh->mNormals != nullptr) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the pModel contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the pModel's faces (a face is a pModel its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        cout << "meshes的大小为" << meshes.size() << endl;
        bones.resize(mesh->mNumVertices);

        if (hasAnimation) {
            //加载骨骼信息
            LoadBones(meshes.size(), mesh, bones);
        }

        unsigned int materialIndex = mesh->mMaterialIndex;

        Texture *pTexture = nullptr;
        if (materialIndex < m_Textures.size()) {
            pTexture = m_Textures[materialIndex];
        }
        // return a pModel object created from the extracted pModel data
        return new Mesh(vertices, indices, bones, pTexture);
    }

    void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const Matrix4f &ParentTransform) {

        string NodeName(pNode->mName.data);
        Matrix4f NodeTransformation(pNode->mTransformation);
        const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, NodeName);

        if (pNodeAnim) {
            // Interpolate scaling and generate scaling transformation matrix
            aiVector3D Scaling;
            CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
            Matrix4f ScalingM;
            ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

            // Interpolate rotation and generate rotation transformation matrix
            aiQuaternion RotationQ;
            CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
            Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

            // Interpolate translation and generate translation transformation matrix
            aiVector3D Translation;
            CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
            Matrix4f TranslationM;
            TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

            // Combine the above transformations
            NodeTransformation = TranslationM * RotationM * ScalingM;
        }

        Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            uint BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation =
                    m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
        }

        for (uint i = 0; i < pNode->mNumChildren; i++) {
            ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
        }
    }

    void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }
        if (pNodeAnim->mPositionKeys == nullptr) {
            return;
        }

        uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
        uint NextPositionIndex = (PositionIndex + 1);
//        assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
        float DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                                   pNodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor = (AnimationTime - (float) pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }


    void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }
        if (pNodeAnim->mRotationKeys == nullptr) {
            return;
        }

        uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        uint NextRotationIndex = (RotationIndex + 1);
//        assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = (float) (pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                                   pNodeAnim->mRotationKeys[RotationIndex].mTime);
        float Factor = (AnimationTime - (float) pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
    }


    void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
        if (pNodeAnim->mNumScalingKeys == 1) {
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }
        if (pNodeAnim->mScalingKeys == nullptr) {
            return;
        }

        uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
        uint NextScalingIndex = (ScalingIndex + 1);
//        assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
        float DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                                   pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        float Factor = (AnimationTime - (float) pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }

    uint FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim) {
        for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
            if (pNodeAnim->mPositionKeys == nullptr) {
                return 0;
            }
            if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
                return i;
            }
        }
//        assert(0);
        return 0;
    }

    uint FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim) {
//        assert(pNodeAnim->mNumRotationKeys > 0);
        for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
            if (pNodeAnim->mRotationKeys == nullptr) {
                return 0;
            }
            if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
//        assert(0);
        return 0;
    }

    uint FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim) {
//        assert(pNodeAnim->mNumScalingKeys > 0);
        for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
            if (pNodeAnim->mScalingKeys == nullptr) {
                return 0;
            }
            if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
                return i;
            }
        }
//        assert(0);
        return 0;
    }

    const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const string NodeName) {
        for (uint i = 0; i < pAnimation->mNumChannels; i++) {
            const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

            if (string(pNodeAnim->mNodeName.data) == NodeName) {
                return pNodeAnim;
            }
        }
        return nullptr;
    }
};

#endif