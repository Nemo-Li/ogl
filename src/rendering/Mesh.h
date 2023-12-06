///**
//* Code taken from www.learnopengl.com
//* Slightly modified by Tomasz Gałaj 2018
//**/
//
//#ifndef MESH_H
//#define MESH_H
//
//#include <glad/glad.h> // holds all OpenGL type declarations
//#include <glm/glm.hpp>
//#include <vector>
//#include "math/math_3d.h"
//
//#define NUM_BONES_PER_VEREX 4
//#define SNPRINTF snprintf
//
//#define N  99
//
//static const uint MAX_BONES = 100;
//
//struct Vertex {
//    // position
//    glm::vec3 Position;
//    // normal
//    glm::vec3 Normal;
//    // texCoords
//    glm::vec2 TexCoords;
//};
//
//struct BoneInfo {
//    Matrix4f BoneOffset;
//    Matrix4f FinalTransformation;
//
//    BoneInfo() {
//        BoneOffset.SetZero();
//        FinalTransformation.SetZero();
//    }
//};
//
//struct VertexBoneData {
//    uint IDs[NUM_BONES_PER_VEREX];
//    float Weights[NUM_BONES_PER_VEREX];
//
//    VertexBoneData() {
//        Reset();
//    };
//
//    void Reset() {
//        ZERO_MEM(IDs);
//        ZERO_MEM(Weights);
//    }
//
//    void AddBoneData(uint BoneID, float Weight);
//};
//
//void VertexBoneData::AddBoneData(uint BoneID, float Weight) {
//    for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
//        if (Weights[i] == 0.0) {
//            IDs[i] = BoneID;
//            Weights[i] = Weight;
//            return;
//        }
//    }
//    // should never get here - more bones than we have space for
//    // 这里可以优化，现在一个顶点只能由4个骨骼来控制
//    // assert(0);
//}
//
//class Mesh {
//public:
//    /*  Mesh Data  */
//    std::vector<Vertex> vertices;
//    std::vector<unsigned int> indices;
//    std::vector<VertexBoneData> bones;
//    Texture *pTexture;
//
//    unsigned int VAO;
//    float random_r;
//    float random_g;
//    float random_b;
//
//    /*  Functions  */
//    // constructor
//    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones,
//         Texture *texture) {
//        this->vertices = vertices;
//        this->indices = indices;
//        this->bones = bones;
//        this->pTexture = texture;
//
//        // now that we have all the required data, set the vertex buffers and its attribute pointers.
//        setupMesh();
//
//        std::srand(GetCurrentTimeMillis());
//
//        random_r = rand() % (N + 1) / (float)(N + 1);
//        random_g = rand() % (N + 1) / (float)(N + 1);
//        random_b = rand() % (N + 1) / (float)(N + 1);
//
//        printf("r g b: %f %f %f \n", random_r, random_g, random_b);
//    }
//
//    // render the pModel
//    void Draw(Shader* shader) {
//        // draw pModel
//        glBindVertexArray(VAO);
//
//        if (pTexture != nullptr) {
//            pTexture->bind();
//        } else {
//            shader->setUniform3fv("mesh_color", glm::vec3(
//                    random_r,
//                    random_g,
//                    random_b
//                    ));
//        }
//
//        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//        glBindVertexArray(0);
//    }
//
//private:
//    /*  Render data  */
//    unsigned int VBO, EBO, BONE;
//
//    /*  Functions    */
//    // initializes all the buffer objects/arrays
//    void setupMesh() {
//        // create buffers/arrays
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//        glGenBuffers(1, &BONE);
//
//        glBindVertexArray(VAO);
//
//        // load data into vertex buffers
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
//
//        // set the vertex attribute pointers
//        // vertex Positions
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
//        // vertex normals
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
//        // vertex texture coords
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));
//
//        glBindBuffer(GL_ARRAY_BUFFER, BONE);
//        glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW);
//
//        glEnableVertexAttribArray(3);
//        glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void *) 0);
//        glEnableVertexAttribArray(4);
//        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
//                              (void *) offsetof(VertexBoneData, Weights));
//        assert(glGetError() == GL_NO_ERROR);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//        assert(glGetError() == GL_NO_ERROR);
//
//        glBindVertexArray(0);
//        assert(glGetError() == GL_NO_ERROR);
//    }
//};
//
//#endif