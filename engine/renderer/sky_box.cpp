//
// Created by Nemo li on 2024/1/6.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "sky_box.h"
#include "shader.h"
#include "camera.h"
#include "component/game_object.h"

RTTR_REGISTRATION {
    rttr::registration::class_<SkyBox>("SkyBox")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

void SkyBox::Render() {
    //从当前Camera获取View Projection
    auto current_camera = Camera::current_camera();
    if (current_camera == nullptr) {
        return;
    }
    //判断相机的 culling_mask 是否包含当前物体 layer
    if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
        return;
    }

    //我们通过取4x4矩阵左上角的3x3矩阵来移除变换矩阵的位移部分
    glm::mat4 view = current_camera->view_mat4();
    view = glm::mat4(glm::mat3(view));

    glm::mat4 projection = current_camera->projection_mat4();

    if (skyboxVAO == 0 || skyboxVBO == 0) {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices_), &skybox_vertices_, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    }

    glEnable(GL_DEPTH_TEST);

    // change depth function so depth test passes when values are equal to depth buffer's content
    glDepthFunc(
            GL_LEQUAL);
    skyboxShader->Use();

    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    // set depth function back to default
    glDepthFunc(GL_LESS);
}

void SkyBox::LoadCubeMap(const char *x_pos, const char *x_neg, const char *y_pos, const char *y_neg, const char *z_pos,
                         const char *z_neg) {
    std::vector<std::string> faces;

    faces.emplace_back(x_pos);
    faces.emplace_back(x_neg);
    faces.emplace_back(y_pos);
    faces.emplace_back(y_neg);
    faces.emplace_back(z_pos);
    faces.emplace_back(z_neg);

    cubeMap = new CubeMap();
    cubeMap->loadCubeMap(faces);

    skyboxShader = Shader::Find("shader/skybox");
    skyboxShader->Use();
    skyboxShader->setInt("skybox", 0);
}

