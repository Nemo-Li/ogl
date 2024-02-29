//
// Created by Nemo li on 2024/2/29.
//

#include "glad/glad.h"
#include "anim_mesh_render.h"
#include <rttr/registration>
#include "shader.h"
#include "camera.h"
#include "component/game_object.h"
#include "component/transform.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Host.h>

RTTR_REGISTRATION {
    rttr::registration::class_<AnimMeshRender>("AnimMeshRender")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

AnimMeshRender::AnimMeshRender() {
    vao_ = 0;
    ebo_ = 0;
    vbo_ = nullptr;

    shader = Shader::Find("shader/basic");
    shader->Use();
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
        m_boneLocation[i] = glGetUniformLocation(shader->program_id(), Name);
    }
}

void AnimMeshRender::Render() {
    auto current_camera = Camera::current_camera();
    if (current_camera == nullptr) {
        return;
    }

    if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
        return;
    }

    glm::mat4 view = current_camera->view_mat4();
    glm::mat4 projection = current_camera->projection_mat4();

    auto component_transform = game_object()->GetComponent<Transform>();
    auto transform = dynamic_cast<Transform *>(component_transform);
    if (!transform) {
        return;
    }

    glm::mat4 trans = glm::translate(transform->position());
    auto rotation = transform->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x),
                                                 glm::radians(rotation.z));
    glm::mat4 scale = glm::scale(transform->scale()); //缩放;
    glm::mat4 model = trans * scale * eulerAngleYXZ;
    glm::mat4 viewProj = projection * view;

    glm::mat3 normalMatrix = glm::inverse(glm::transpose(glm::mat3(model)));

    if (vao_ == 0) {
        vbo_ = new unsigned int[6];
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        glGenBuffers(6, vbo_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(glm::vec3), &animation_->aVertices[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
        printf("aVertices size: %lu\n", animation_->aVertices.size());
        glEnableVertexAttribArray(0); __CHECK_GL_ERROR__

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(glm::vec3), &animation_->aNormals[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);
        printf("aNormals size: %lu\n", animation_->aNormals.size());
        glEnableVertexAttribArray(1); __CHECK_GL_ERROR__


        glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(glm::vec2), &animation_->aTexcoords[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, nullptr);
        printf("aTexcoords size: %lu\n", animation_->aTexcoords.size());
        glEnableVertexAttribArray(2); __CHECK_GL_ERROR__

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[3]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(unsigned int), &animation_->aMids[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, false, 0, nullptr); __CHECK_GL_ERROR__
        printf("aMids size: %lu\n", animation_->aMids.size());
        glEnableVertexAttribArray(3); __CHECK_GL_ERROR__

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[4]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(unsigned int) * 4, &animation_->aBoneids[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(4, 4, GL_UNSIGNED_INT, false, 0, nullptr); __CHECK_GL_ERROR__
        printf("aBoneids size: %lu\n", animation_->aBoneids.size());
        glEnableVertexAttribArray(4); __CHECK_GL_ERROR__

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[5]); __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, animation_->vertCount * sizeof(glm::vec4), &animation_->aWeights[0],
                     GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glVertexAttribPointer(5, 4, GL_FLOAT, false, 0, nullptr); __CHECK_GL_ERROR__
        printf("aWeights size: %lu\n", animation_->aWeights.size());
        glEnableVertexAttribArray(5); __CHECK_GL_ERROR__

        glGenBuffers(1, &ebo_); __CHECK_GL_ERROR__
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_); __CHECK_GL_ERROR__
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, animation_->aIndices.size() * sizeof(unsigned int),
                     &animation_->aIndices[0], GL_STATIC_DRAW); __CHECK_GL_ERROR__
        glBindVertexArray(0); __CHECK_GL_ERROR__
    }

    shader->Use(); __CHECK_GL_ERROR__
    shader->setMat4("model", model); __CHECK_GL_ERROR__
    shader->setMat4("viewProj", viewProj); __CHECK_GL_ERROR__
    shader->setVec3("cam_pos", current_camera->pos_vec3()); __CHECK_GL_ERROR__
    shader->setMat3("normalMatrix", normalMatrix); __CHECK_GL_ERROR__
    shader->setInt("textureArray", 0);
    animation_->texture3D->use(0);

    glBindVertexArray(vao_); __CHECK_GL_ERROR__
    glDrawElements(GL_TRIANGLES, animation_->aIndices.size(), GL_UNSIGNED_INT, nullptr); __CHECK_GL_ERROR__
    glBindVertexArray(0); __CHECK_GL_ERROR__
}

void AnimMeshRender::SetAnimation(AssAnimation *animation) {
    animation_ = animation;
    Transforms.resize(animation_->boneCount);
    for (int i = 0; i < animation_->boneCount; i++) {
        glm::mat4 f = glm::mat4(1.0);
        Transforms[i] = f;
    }

    for (uint i = 0; i < Transforms.size(); i++) {
        glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, &Transforms[i][0][0]);
    }
}
