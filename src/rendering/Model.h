/** 
 * Code taken from www.learnopengl.com
 * Slightly modified by Tomasz Gałaj 2018
 **/

#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
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
    std::vector<Mesh> meshes;
    std::string directory;

    map<string, uint> m_BoneMapping; // maps a bone name to its index
    vector<BoneInfo> m_BoneInfo;

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
            meshes[i].Draw();
        }
    }

    uint getBoneNum() {
        return m_NumBones;
    }

private:
    uint m_NumBones = 0;
    aiMatrix4x4 m_GlobalInverseTransform;

    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path) {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(ROOT_DIR + path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                                  aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        //根节点的转换矩阵
        m_GlobalInverseTransform = scene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        std::cout << "mNumMeshes--" << scene->mNumMeshes << std::endl;
        //记录所有mesh的
        m_Entries.resize(scene->mNumMeshes);
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

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
        initMaterial(scene);

    }

    void initMaterial(const aiScene *scene) {
        // Initialize the materials
        for (uint i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial *pMaterial = scene->mMaterials[i];

            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString Path;

                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) ==
                    AI_SUCCESS) {
                    std::string p(Path.data);

                    if (p.substr(0, 2) == ".\\") {
                        p = p.substr(2, p.size() - 2);
                    }

                    std::string FullPath = p;
                    printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
                }
            }
        }
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene) {

        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
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
            cout << "骨骼名称:" << pMesh->mBones[i]->mName.data << endl;

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
                Bones[VertexID].AddBoneData(BoneIndex, Weight);
            }
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<VertexBoneData> bones;

        // Walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
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
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        cout << "meshes的大小为" << meshes.size() << endl;
        bones.resize(mesh->mNumVertices);
        //加载骨骼信息
        LoadBones(meshes.size(), mesh, bones);

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, bones);
    }
};

#endif