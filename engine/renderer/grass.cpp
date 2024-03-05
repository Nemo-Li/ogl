//
// Created by Nemo li on 2024/2/4.
//

#include "grass.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "camera.h"
#include "shader.h"
#include "glm/glm.hpp"
#include "component/game_object.h"
#include <vector>
#include <component/transform.h>
#include <framework/application.h>
#include "texture2d.h"
#include <glm/gtx/string_cast.hpp>
#include <helpers/time.h>
#include "light.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Grass>("grass")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

void Grass::Render() {
    Camera *current_camera = Camera::current_camera();
    if (current_camera == nullptr) {
        return;
    }
    //判断相机的 culling_mask 是否包含当前物体 layer
    if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
        return;
    }

    glm::mat4 view = current_camera->view_mat4();
    glm::mat4 projection = current_camera->projection_mat4();
    glm::mat4 model = glm::mat4(1.0);
//    printf("model: %s\n", glm::to_string(model).c_str());

    float lightStrength = 1.0f;
    auto go_light = GameObject::Find("light");
    if (go_light) {
        auto light = go_light->GetComponent<Light>();
        lightStrength = light->lightStrength();
    }

    glm::mat4 vp_mat4 = projection * view;
    Shader *tessHeightMapShader = material_->shader();

    if (vao_ == 0) {
        // first, configure the cube's VAO (and terrainVBO)
        unsigned int terrainVBO;
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &terrainVBO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        // texCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    __CHECK_GL_ERROR__

    tessHeightMapShader->Use();
    glEnable(GL_DEPTH_TEST);

    // world transformation
    tessHeightMapShader->setMat4("model", model);
    tessHeightMapShader->setMat4("projection", projection);
    tessHeightMapShader->setMat4("view", view);
    tessHeightMapShader->setMat4("viewProjectMatrix", vp_mat4);
    tessHeightMapShader->setFloat("lightStrength", lightStrength);

    float time = Time::TimeSinceStartup();
    tessHeightMapShader->setFloat("time", time);

    __CHECK_GL_ERROR__

    //高度图纹理，暂时单独处理（与其它不同，后续考虑是否合并到材质中）
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    //从Pass节点拿到保存的Texture
    std::vector<std::pair<std::string, Texture2D *>> textures = material_->textures();
    for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
        tessHeightMapShader->setInt(textures[texture_index].first, texture_index + 1);
        textures[texture_index].second->bind(texture_index + 1);
    }

    __CHECK_GL_ERROR__
    // render the terrain
    glBindVertexArray(vao_);
    __CHECK_GL_ERROR__
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
    __CHECK_GL_ERROR__
    glBindVertexArray(0);
    __CHECK_GL_ERROR__
}

void Grass::LoadTerrainTexture(const std::string &terrainTex) {
    // load and create a texture
    // -------------------------
    glGenTextures(1, &texture);
    __CHECK_GL_ERROR__
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Shader *heightMapShader = material()->shader();
    heightMapShader->Use();

    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load((Application::data_path() + terrainTex).c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        heightMapShader->setInt("heightMap", 0);
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    for (unsigned i = 0; i <= rez - 1; i++) {
        for (unsigned j = 0; j <= rez - 1; j++) {
            vertices.push_back(-float(width) / 2.0f + float(width) * i / (float) rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float) rez); // v.z
            vertices.push_back(i / (float) rez); // u
            vertices.push_back(j / (float) rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float) rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float) rez); // v.z
            vertices.push_back((i + 1) / (float) rez); // u
            vertices.push_back(j / (float) rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float) rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float) rez); // v.z
            vertices.push_back(i / (float) rez); // u
            vertices.push_back((j + 1) / (float) rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float) rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float) rez); // v.z
            vertices.push_back((i + 1) / (float) rez); // u
            vertices.push_back((j + 1) / (float) rez); // v
        }
    }
    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;
}

Grass::Grass() : material_(nullptr) {
    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;
    glEnable(GL_DEPTH_TEST);
}
