//
// Created by Nemo li on 2023/12/6.
//

#include "mesh_renderer.h"
#include <rttr/registration>
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glad/glad.h>
#include "component/game_object.h"
#include "component/transform.h"
#include "mesh_filter.h"
#include "material.h"
#include "shader.h"
#include "texture2d.h"
#include "camera.h"

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
    //从当前Camera获取View Projection
    auto current_camera = Camera::current_camera();
    if (current_camera == nullptr) {
        return;
    }
    //判断相机的 culling_mask 是否包含当前物体 layer
    if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
        return;
    }

    glm::mat4 view = current_camera->view_mat4();
    glm::mat4 projection = current_camera->projection_mat4();

    //主动获取 Transform 组件，计算mvp。
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
    glm::mat4 mvp = projection * view * model;

    //主动获取 MeshFilter 组件
    auto component_mesh_filter = game_object()->GetComponent<MeshFilter>();
    auto mesh_filter = dynamic_cast<MeshFilter *>(component_mesh_filter);
    if (!mesh_filter) {
        return;
    }
    //当骨骼蒙皮动画生效时，渲染骨骼蒙皮Mesh
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
        mesh = m->GetMesh();
        shader = m->shader;
    }

    if (vao_ == 0) {
        //在GPU上创建缓冲区对象
        glGenBuffers(1, &vbo_);__CHECK_GL_ERROR__
        //将缓冲区对象指定为顶点缓冲区对象
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);__CHECK_GL_ERROR__
        //上传顶点数据到缓冲区对象
        glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * sizeof(Vertex), mesh->vertex_data_,
                     GL_DYNAMIC_DRAW);__CHECK_GL_ERROR__

        if (mesh->textureIdBuffer) {
            glGenBuffers(1, &texture_vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_);
            glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * 1 * sizeof(short), mesh->textureIdBuffer,
                         GL_STATIC_DRAW);__CHECK_GL_ERROR__
        }

        //在GPU上创建缓冲区对象
        glGenBuffers(1, &ebo_);
        //将缓冲区对象指定为顶点索引缓冲区对象
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        //上传顶点索引数据到缓冲区对象
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->vertex_index_num_ * sizeof(unsigned short),
                     mesh->vertex_index_data_, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao_);
        //设置VAO
        glBindVertexArray(vao_); __CHECK_GL_ERROR__
        {
            //指定当前使用的VBO
            glBindBuffer(GL_ARRAY_BUFFER, vbo_); __CHECK_GL_ERROR__
            //将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0); __CHECK_GL_ERROR__
            glEnableVertexAttribArray(0);
            //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
            glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex),
                                  (void *) (sizeof(float) * 3)); __CHECK_GL_ERROR__
            glEnableVertexAttribArray(1);
            //将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联，最后的0表示数据偏移量。
            glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex),
                                  (void *) (sizeof(float) * (3 + 4))); __CHECK_GL_ERROR__
            glEnableVertexAttribArray(2);

            if (mesh->textureIdBuffer) {
                glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_);__CHECK_GL_ERROR__
                glVertexAttribPointer(3, 1, GL_SHORT, GL_FALSE, 0, (void *) 0);__CHECK_GL_ERROR__
                glEnableVertexAttribArray(3);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);__CHECK_GL_ERROR__
        }
        glBindVertexArray(0); __CHECK_GL_ERROR__
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_); __CHECK_GL_ERROR__
        //更新Buffer数据
        glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->vertex_num_ * sizeof(Vertex), mesh->vertex_data_); __CHECK_GL_ERROR__
    }

    shader->Use();

    {
        if (current_camera->camera_use_for() == Camera::CameraUseFor::SCENE) {
            glEnable(GL_DEPTH_TEST); __CHECK_GL_ERROR__
        } else {
            glDisable(GL_DEPTH_TEST); __CHECK_GL_ERROR__
        }
        //开启背面剔除
//        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND); __CHECK_GL_ERROR__
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); __CHECK_GL_ERROR__

        //上传mvp矩阵
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        if (material_) {
            //从Pass节点拿到保存的Texture
            std::vector<std::pair<std::string, Texture2D *>> textures = material_->textures();
            for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
                shader->setInt(textures[texture_index].first, texture_index);
                textures[texture_index].second->bind(texture_index);
            }
        }

        if (filterType == MODEL) {
            mesh_filter->GetModel()->texture3D->use(0);__CHECK_GL_ERROR__
        }

        glBindVertexArray(vao_);
        {
            //使用顶点索引进行绘制，最后的0表示数据偏移量。
            glDrawElements(GL_TRIANGLES, mesh->vertex_index_num_, GL_UNSIGNED_SHORT, 0);
        }
        glBindVertexArray(0); __CHECK_GL_ERROR__
        glBindTexture(GL_TEXTURE_2D, 0); __CHECK_GL_ERROR__
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0); __CHECK_GL_ERROR__
    }
}

void MeshRenderer::SetMaterial(Material *material) {
    material_ = material;
}

void MeshRenderer::OnDestroy() {

}
