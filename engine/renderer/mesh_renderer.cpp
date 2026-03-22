//
// Created by Nemo li on 2023/12/6.
//

#include "mesh_renderer.h"
#include <rttr/registration>
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glad/glad.h>
#include <helpers/time.h>
#include "component/game_object.h"
#include "component/transform.h"
#include "mesh_filter.h"
#include "material.h"
#include "shader.h"
#include "texture2d.h"
#include "camera.h"
#include "light.h"
#include "renderer/render_queue.h"

RTTR_REGISTRATION {
    rttr::registration::class_<MeshRenderer>("MeshRenderer")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

MeshRenderer::MeshRenderer() {
    material_ = nullptr;
}

MeshRenderer::~MeshRenderer() {
    printf("mesh render release\n");
    if (material_) {
        delete material_;
        material_ = nullptr;
    }
}

void MeshRenderer::Render() {
    RenderQueue::Push({[this]() {
        // 从当前 Camera 获取 View Projection
        auto current_camera = Camera::current_camera();
        if (current_camera == nullptr) {
            return;
        }
        // 判断相机的 culling_mask 是否包含当前物体 layer
        if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
            return;
        }

        glm::mat4 view = current_camera->view_mat4();
        glm::mat4 projection = current_camera->projection_mat4();

        // 主动获取 Transform 组件，计算 mvp
        auto component_transform = game_object()->GetComponent<Transform>();
        auto transform = dynamic_cast<Transform *>(component_transform);
        if (!transform) {
            return;
        }

        glm::mat4 trans = glm::translate(transform->position());
        auto rotation = transform->rotation();
        glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x),
                                                     glm::radians(rotation.z));
        glm::mat4 scale = glm::scale(transform->scale());
        glm::mat4 model = trans * scale * eulerAngleYXZ;

        // 主动获取 MeshFilter 组件
        auto component_mesh_filter = game_object()->GetComponent<MeshFilter>();
        auto mesh_filter = dynamic_cast<MeshFilter *>(component_mesh_filter);
        if (!mesh_filter) {
            return;
        }

        float lightStrength = 1.0f;
        glm::vec3 lightDir = glm::vec3(0, 1, 0.2f);
        auto go_light = GameObject::Find("light");
        if (go_light) {
            auto light = go_light->GetComponent<Light>();
            if (light) {
                lightStrength = light->lightStrength();
                lightDir = light->lightDir();
            }
        }

        // 渲染 Mesh
        int filterType = mesh_filter->GetMeshFilterType();
        Mesh *mesh = mesh_filter->mesh();

        Shader *shader = nullptr;
        if (material_) {
            shader = material_->shader();
        }

        if (filterType == SKIN_MESH) {
            mesh = mesh_filter->skinned_mesh();
        }

        if (filterType == MODEL) {
            Model *m = mesh_filter->GetModel();
            if (m) {
                mesh = m->GetMesh();
                shader = m->shader;
            }
        }

        if (mesh == nullptr || shader == nullptr) {
            return;
        }

        if (vao_ == 0) {
            glGenBuffers(1, &vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * sizeof(Vertex), mesh->vertex_data_, GL_DYNAMIC_DRAW);

            if (mesh->textureIdBuffer) {
                glGenBuffers(1, &texture_vbo_);
                glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_);
                glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * sizeof(int), mesh->textureIdBuffer, GL_STATIC_DRAW);
            }

            glGenBuffers(1, &ebo_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->vertex_index_num_ * sizeof(unsigned int),
                         mesh->vertex_index_data_, GL_STATIC_DRAW);

            glGenVertexArrays(1, &vao_);
            glBindVertexArray(vao_);
            {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), (void *) (sizeof(float) * 3));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void *) (sizeof(float) * (3 + 4)));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void *) (sizeof(float) * (3 + 4 + 2)));
                glEnableVertexAttribArray(3);

                if (mesh->textureIdBuffer) {
                    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_);
                    glVertexAttribIPointer(4, 1, GL_INT, 0, (void *) 0);
                    glEnableVertexAttribArray(4);
                }

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            }
            glBindVertexArray(0);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->vertex_num_ * sizeof(Vertex), mesh->vertex_data_);
        }

        shader->Use();

        if (current_camera->camera_use_for() == Camera::CameraUseFor::SCENE) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("viewProjectMatrix", projection * view);
        shader->setFloat("time", Time::TimeSinceStartup());
        shader->setVec3("eyePos", current_camera->pos_vec3());
        shader->setFloat("lightStrength", lightStrength);
        shader->setVec3("lightDir", lightDir);

        if (material_) {
            if (material_->GetCubeMap()) {
                shader->setInt("textureEnv", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, material_->GetCubeMap()->textureID);
                shader->setVec2("waterBias", glm::vec2(0.05, -0.05));
            } else {
                auto &textures = material_->textures();
                for (int i = 0; i < textures.size(); ++i) {
                    if (textures[i].second) {
                        shader->setInt(textures[i].first, i);
                        textures[i].second->bind(i);
                    }
                }
            }
        }

        if (filterType == MODEL && mesh_filter->GetModel() && mesh_filter->GetModel()->texture3D) {
            shader->setInt("textureArray", 0);
            mesh_filter->GetModel()->texture3D->use(0);
        }

        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, mesh->vertex_index_num_, GL_UNSIGNED_INT, 0);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindVertexArray(0);
    }});
}

void MeshRenderer::SetMaterial(Material *material) {
    material_ = material;
}

void MeshRenderer::OnDestroy() {
}
