//
// Created by Nemo li on 2024/2/4.
//

#include "water.h"

Water::Water(int size, float height) {
    waterSize = size;
    waterHeight = height;

    mesh = new Mesh();
    mesh->vertex_num_ = waterSize * waterSize;
    mesh->vertex_data_ = new Vertex[mesh->vertex_num_];
    mesh->vertex_index_num_ = (waterSize - 1) * (waterSize - 1) * 6;
    mesh->vertex_index_data_ = new unsigned short[mesh->vertex_index_num_];
    initFaces();
}

Water::~Water() {

}

void Water::initFaces() {
    int currentVertex = 0;
    int stepCount = waterSize - 1;

    float x, y, z, u, v;
    for (int i = 0, row = 0; row < stepCount + 1; i++, row++) {
        for (int j = 0, col = 0; col < stepCount + 1; j++, col++) {
            x = j;
            z = i;
            y = 16;
            u = col / 4.0;
            v = row / 4.0;
            Vertex vertex = Vertex();
            vertex.position_ = glm::vec3(x, y, z);
            vertex.uv_ = glm::vec2(u, v);
            vertex.color_ = glm::vec4(1.0);

            mesh->vertex_data_[currentVertex] = vertex;
            currentVertex++;
        }
    }

    if (waterSize >= 2) {
        mesh->vertex_data_[0].position_.y = waterHeight;
        mesh->vertex_data_[1].position_.y = -waterHeight;
    }

    int currentIndex = 0, blockFirstIndex = 0;
    int sideVertexCount = stepCount + 1;
    for (int i = 0; i < stepCount; i++) {
        for (int j = 0; j < stepCount; j++) {
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex;
            currentIndex++;
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex + sideVertexCount;
            currentIndex++;
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex + sideVertexCount + 1;
            currentIndex++;
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex;
            currentIndex++;
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex + sideVertexCount + 1;
            currentIndex++;
            mesh->vertex_index_data_[currentIndex] = blockFirstIndex + 1;
            currentIndex++;
            if (j < stepCount - 1)
                blockFirstIndex++;
            else
                blockFirstIndex += 2;
        }
    }
}
