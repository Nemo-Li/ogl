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

RTTR_REGISTRATION {
    rttr::registration::class_<Terrain>("Terrain")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

void Terrain::Render() {
    Camera *current_camera = Camera::current_camera();

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
//    printf("terrain rotation %s \n", glm::to_string(rotation).c_str());
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x),
                                                 glm::radians(rotation.z));
    glm::mat4 scale = glm::scale(transform->scale()); //缩放;
    glm::mat4 model = trans * scale * eulerAngleYXZ;
//    printf("model: %s\n", glm::to_string(model).c_str());

    Shader *heightMapShader = material_->shader();

    if (vao_ == 0) {
        GLuint vbo, ebo;
        glGenVertexArrays(1, &vao_);
        //设置VAO
        glBindVertexArray(vao_);
        //在GPU上创建缓冲区对象
        glGenBuffers(1, &vbo);
        //将缓冲区对象指定为顶点缓冲区对象
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //上传顶点数据到缓冲区对象
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(float),       // size of vertices buffer
                     &vertices[0],                          // pointer to first element
                     GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // position attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));
        glEnableVertexAttribArray(1);


        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int), // size of indices buffer
                     &indices[0],                           // pointer to first element
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    heightMapShader->Use();
    glEnable(GL_DEPTH_TEST);

    // world transformation
    heightMapShader->setMat4("model", model);
    heightMapShader->setMat4("projection", projection);
    heightMapShader->setMat4("view", view);

    //从Pass节点拿到保存的Texture
    std::vector<std::pair<std::string, Texture2D *>> textures = material_->textures();

    for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
        GLint u_texture_location = glGetUniformLocation(heightMapShader->program_id(),
                                                        textures[texture_index].first.c_str());
        //激活纹理单元0
        glActiveTexture(GL_TEXTURE0 + texture_index);
        //将加载的图片纹理句柄，绑定到纹理单元0的Texture2D上。
        glBindTexture(GL_TEXTURE_2D, textures[texture_index].second->texture_id());
        //设置Shader程序从纹理单元0读取颜色数据
        glUniform1i(u_texture_location, texture_index);
    }

    // render the cube
    glBindVertexArray(vao_);
    for (unsigned strip = 0; strip < numStrips; strip++) {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                       numTrisPerStrip + 2,   // number of indices to render
                       GL_UNSIGNED_INT,     // index data type
                       (void *) (sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
    }
}

void Terrain::LoadTerrainTexture(const std::string &terrainTex) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load((Application::data_path() + terrainTex).c_str(), &width,
                                    &height, &nrChannels, 0);
    if (data) {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    unsigned bytePerPixel = nrChannels;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char *pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            // vx
            vertices.push_back(-width / 2.0f + width * j / (float) width);
            float elevation = (int) y * yScale - yShift;
            // vy
            vertices.push_back(elevation);
            elevation_vec.push_back(elevation);
            // vz
            vertices.push_back(-height / 2.0f + height * i / (float) height);

            // uv
            vertices.push_back(float(i) / height * 16); //u
            vertices.push_back(float(j) / width * 16); //v
        }
    }
    std::cout << "Loaded " << vertices.size() / 5 << " vertices" << std::endl;
    stbi_image_free(data);

    int rez = 1;
    for (unsigned i = 0; i < height - 1; i += rez) {
        for (unsigned j = 0; j < width; j += rez) {
            for (unsigned k = 0; k < 2; k++) {
                indices.push_back(j + width * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    numStrips = (height - 1) / rez;
    numTrisPerStrip = (width / rez) * 2 - 2;

    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each"
              << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;
}

Terrain::Terrain() : material_(nullptr) {}

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

