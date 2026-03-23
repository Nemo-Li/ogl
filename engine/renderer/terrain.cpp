//
// Created by Nemo li on 2023/12/18.
//

#include "glad/glad.h"
#include "terrain.h"
#include "stb_image.h"
#include "camera.h"
#include "shader.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "component/game_object.h"
#include <vector>
#include <component/transform.h>
#include <framework/application.h>
#include <glm/gtx/string_cast.hpp>
#include "texture2d.h"
#include "light.h"
#include "renderer/render_queue.h"
#include "framework/application.h"
#include "framework/scene.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Terrain>("Terrain")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

void Terrain::Render() {
    RenderQueue::Push({[this]() {
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

        //主动获取 Transform 组件，计算mvp。
        auto component_transform = game_object()->GetComponent<Transform>();
        auto transform = dynamic_cast<Transform *>(component_transform);
        if (!transform) {
            return;
        }

        float lightStrength = 1.0f;
        auto go_light = GameObject::Find("light");
        if (go_light) {
            auto light = go_light->GetComponent<Light>();
            lightStrength = light->lightStrength();
        }

        glm::mat4 trans = glm::translate(transform->position());
        auto rotation = transform->rotation();
//    printf("terrain rotation %s \n", glm::to_string(rotation).c_str());
        glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x),
                                                     glm::radians(rotation.z));
        glm::mat4 scale = glm::scale(transform->scale()); //缩放;
        glm::mat4 model = trans * scale * eulerAngleYXZ;
//    printf("model: %s\n", glm::to_string(model).c_str());

        Shader *tessHeightMapShader = material_->shader();

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    printf("terrain %d\n", tessHeightMapShader->program_id());
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
        } else {
            __CHECK_GL_ERROR__

            tessHeightMapShader->Use();
            glEnable(GL_DEPTH_TEST);

            // world transformation
            tessHeightMapShader->setMat4("model", model);
            tessHeightMapShader->setMat4("projection", projection);
            tessHeightMapShader->setMat4("view", view);
            tessHeightMapShader->setFloat("lightStrength", lightStrength);
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

            glBindVertexArray(vao_);
            glDrawArrays(GL_PATCHES, 0, vertices.size() / 5);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }});
}

void Terrain::LoadTerrainTexture(const std::string &terrainTex) {
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

        // Set AABB for terrain
        if (game_object()) {
            float halfW = float(width) / 2.0f;
            float halfH = float(height) / 2.0f;
            // Elevation is roughly between -16 and 48 based on the code below
            game_object()->set_local_aabb(AABB(glm::vec3(-halfW, -32.0f, -halfH), glm::vec3(halfW, 64.0f, halfH)));
            game_object()->UpdateAABB();
            
            // 地形也建议标记为始终可见，或者通过大包围盒正确剔除
            if (Application::scene()) {
                Application::scene()->SetAlwaysVisible(game_object(), true);
            }
        }
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    unsigned bytePerPixel = nrChannels;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char *pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];
            float elevation = (int) y * yScale - yShift;
            elevation_vec.push_back(elevation);
        }
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

Terrain::Terrain() : material_(nullptr) {
    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;
    glEnable(GL_DEPTH_TEST);
}

float Terrain::GetElevation(int x, int z) {
    if (x < -width / 2 || x > width / 2) {
        return 0;
    }
    if (z < -height / 2 || z > height / 2) {
        return 0;
    }
    int index = (z + height / 2) * width + (x + width / 2);
    return elevation_vec[index];
}

