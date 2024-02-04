//
// Created by Nemo li on 2024/1/19.
//

#include "glm/glm.hpp"
#include <glad/glad.h>
#include "instance_render.h"
#include "camera.h"
#include "component/game_object.h"
#include "material.h"
#include "shader.h"
#include "texture2d.h"

RTTR_REGISTRATION {
    rttr::registration::class_<InstanceRender>("InstanceRender")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

InstanceRender::InstanceRender() {
//    board = new Board(5.0, 5.0, 5.0, 0, 0);
//    mesh = board->mesh();
    tree_model = new Model("model/firc/firc", 2);
    mesh = tree_model->GetMesh();
}

InstanceRender::~InstanceRender() {
    if (board) {
        delete board;
        board = nullptr;
    }
    if (mesh) {
        delete mesh;
        mesh = nullptr;
    }
    if (position_) {
        delete position_;
        position_ = nullptr;
    }
}

void InstanceRender::Render() {
    Camera *current_camera = Camera::current_camera();
    if (current_camera == nullptr) {
        return;
    }

    glm::mat4 view = current_camera->view_mat4();
    glm::mat4 projection = current_camera->projection_mat4();

    Shader *shader = material()->shader_;
    glm::mat4 vp_mat = projection * view;
    glm::vec3 right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::mat4 model = glm::mat4(1.0f);

    if (vao_ == 0) {
        unsigned int vertexVbo, ebo, pos_vbo, texture_vbo;
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vertexVbo);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->vertex_num_, mesh->vertex_data_, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (7 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &pos_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 3, position_, GL_STREAM_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(2);

        if (mesh->textureIdBuffer) {
            glGenBuffers(1, &texture_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
            glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * 1 * sizeof(short), mesh->textureIdBuffer,
                         GL_STATIC_DRAW);
            glVertexAttribPointer(3, 1, GL_SHORT, GL_FALSE, 0, (void *) 0);
            glEnableVertexAttribArray(3);
        }

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * mesh->vertex_index_num_,
                     mesh->vertex_index_data_, GL_STATIC_DRAW);

        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 1);

        glBindVertexArray(0);
    }

    shader->Use();
    shader->setMat4("viewProjectMatrix", vp_mat);
    shader->setVec3("viewRight", right);
    shader->setMat4("model", model);
    shader->setBool("billborad", false);
    shader->setInt("textureArray", 1);
    

    //从Pass节点拿到保存的Texture
//    std::vector<std::pair<std::string, Texture2D *>> textures = material()->textures();
//    for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
//        shader->setInt(textures[texture_index].first, texture_index);
//        textures[texture_index].second->bind(texture_index);
//    }

    tree_model->texture3D->use(1);

    glBindVertexArray(vao_);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->vertex_index_num_, GL_UNSIGNED_SHORT, (void *) 0, count);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void InstanceRender::InitInstance(Terrain *terrain) {
    position_ = new float[3 * count];
    for (int i = 0; i < count; ++i) {
        float x = rand() % 100;
        float z = rand() % 100;
        position_[i * 3 + 0] = x;
        position_[i * 3 + 2] = z;
        position_[i * 3 + 1] = terrain->GetElevation((int) x, (int) z) /*+ 2.5f*/;
    }
}
