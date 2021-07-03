/**
* Code taken from www.learnopengl.com
* Slightly modified by Tomasz Gałaj 2018
**/

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <vector>
#include "math/math_3d.h"

#define NUM_BONES_PER_VEREX 8
#define SNPRINTF snprintf

static const uint MAX_BONES = 100;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct BoneInfo {
    Matrix4f BoneOffset;
    Matrix4f FinalTransformation;

    BoneInfo() {
        BoneOffset.SetZero();
        FinalTransformation.SetZero();
    }
};

struct VertexBoneData {
    uint IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    VertexBoneData() {
        Reset();
    };

    void Reset() {
        ZERO_MEM(IDs);
        ZERO_MEM(Weights);
    }

    void AddBoneData(uint BoneID, float Weight);
};


void VertexBoneData::AddBoneData(uint BoneID, float Weight) {
    for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
        if (Weights[i] == 0.0) {
            IDs[i] = BoneID;
            Weights[i] = Weight;
            return;
        }
    }
    // should never get here - more bones than we have space for
    assert(0);
}

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<VertexBoneData> bones;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones) {
        this->vertices = vertices;
        this->indices = indices;
        this->bones = bones;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }


    // render the mesh
    void Draw() {
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO, BONE;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh() {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &BONE);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

//        for (int i = 0; i < bones.size(); i++) {
//            std::cout << "第" << i << "个" << std::endl;
//            for (auto weight : bones[i].Weights) {
//                std::cout << weight << std::endl;
//            }
//        }

        glBindBuffer(GL_ARRAY_BUFFER, BONE);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid *) 0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid *) 32);
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid *) 16);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid *) 48);
        assert(glGetError() == GL_NO_ERROR);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
        assert(glGetError() == GL_NO_ERROR);
    }
};

#endif