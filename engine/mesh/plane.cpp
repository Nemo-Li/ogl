//
// Created by Nemo li on 2024/1/16.
//

#include "plane.h"

Plane::Plane() : Plane(1.0f, 1.0f, 1.0f) {

}

Plane::~Plane() {
    if (mesh_) {
        delete mesh_;
        mesh_ = nullptr;
    }
}

Plane::Plane(float baseX, float baseY, float baseZ) : baseX(baseX), baseY(baseY), baseZ(baseZ) {
    mesh_ = new Mesh();
    mesh_->name_ = "quad";
    mesh_->vertex_num_ = 4;
    mesh_->vertex_data_ = new Vertex[mesh_->vertex_num_];
    mesh_->vertex_index_num_ = 6;
    mesh_->vertex_index_data_ = new unsigned short[mesh_->vertex_index_num_];
    initMesh();
}

void Plane::initMesh() {
    Vertex vertex0 = Vertex();
    vertex0.position_ = glm::vec3(-0.5f, 0.0f, -0.5f);
    vertex0.uv_ = glm::vec2(0, 1);
    vertex0.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mesh_->vertex_data_[0] = vertex0;

    Vertex vertex1 = Vertex();
    vertex1.position_ = glm::vec3(-0.5, 0, 0.5);
    vertex1.uv_ = glm::vec2(0, 0);
    vertex1.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mesh_->vertex_data_[1] = vertex1;

    Vertex vertex2 = Vertex();
    vertex2.position_ = glm::vec3(0.5, 0, 0.5);
    vertex2.uv_ = glm::vec2(1, 0);
    vertex2.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mesh_->vertex_data_[2] = vertex2;

    Vertex vertex3 = Vertex();
    vertex3.position_ = glm::vec3(0.5, 0, -0.5);
    vertex3.uv_ = glm::vec2(1, 1);
    vertex3.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mesh_->vertex_data_[3] = vertex3;

    mesh_->vertex_index_data_[0] = 0;
    mesh_->vertex_index_data_[1] = 1;
    mesh_->vertex_index_data_[2] = 2;
    mesh_->vertex_index_data_[3] = 0;
    mesh_->vertex_index_data_[4] = 2;
    mesh_->vertex_index_data_[5] = 3;

    for (int i = 0; i < 4; i++) {
        mesh_->vertex_data_[i].position_.x *= baseX;
        mesh_->vertex_data_[i].position_.y *= baseY;
        mesh_->vertex_data_[i].position_.z *= baseZ;
    }
}
