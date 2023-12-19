//
// Created by Nemo li on 2023/12/18.
//

#include "glad/glad.h"
#include "terrain.h"
#include "stb_image.h"
#include "helpers/root_dir.h"
#include "camera.h"
#include "shader.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include "app/application.h"
#include "component/game_object.h"
#include <vector>
#include <render_async/gpu_resource_mapper.h>
#include <render_async/render_task_producer.h>
#include <component/transform.h>

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
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x),
                                                 glm::radians(rotation.z));
    glm::mat4 scale = glm::scale(transform->scale()); //缩放;
    glm::mat4 model = trans * scale * eulerAngleYXZ;
//    printf("model: %s\n", glm::to_string(model).c_str());

    Shader *heightMapShader = material_->shader();
    unsigned int shader_program_handle = heightMapShader->shader_program_handle();

    if (vertex_array_object_handle_ == 0) {
        vertex_array_object_handle_ = GPUResourceMapper::GenerateVAOHandle();
        vertex_buffer_object_handle_ = GPUResourceMapper::GenerateVBOHandle();
        //发出任务：创建VAO
        RenderTaskProducer::ProduceRenderTaskCreateVAO(shader_program_handle, vertex_array_object_handle_,
                                                       vertex_buffer_object_handle_,
                                                       vertices.size() * sizeof(float),
                                                       3 * sizeof(float), &vertices[0],
                                                       indices.size() * sizeof(unsigned),
                                                       &indices[0]);
    }

    heightMapShader->Use();

    // PreRender
    game_object()->ForeachComponent([](Component *component) {
        component->OnPreRender();
    });

    RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_DEPTH_TEST, current_camera->camera_use_for() ==
                                                                       Camera::CameraUseFor::SCENE);
//    RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_CULL_FACE, true);
//    RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_BLEND, true);
//    RenderTaskProducer::ProduceRenderTaskSetBlenderFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //上传矩阵
    RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "projection", false, projection);
    RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "view", false, view);
    RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "model", false, model);

    // 绑定VAO并绘制
    RenderTaskProducer::ProduceRenderTaskBindVAOAndDrawElementsStrip(vertex_array_object_handle_, numTrisPerStrip,
                                                                     numStrips);

    // PostRender
    game_object()->ForeachComponent([](Component *component) {
        component->OnPostRender();
    });

//    render the cube
//    GLuint vao = GPUResourceMapper::GetVAO(vertex_array_object_handle_);
//    glBindVertexArray(vao);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    for (unsigned strip = 0; strip < numStrips; strip++) {
//        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
//                       numTrisPerStrip + 2,   // number of indices to render
//                       GL_UNSIGNED_INT,     // index data type
//                       (void *) (sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
//    }
}

void Terrain::LoadTerrainTexture(const std::string &terrainTex) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load((Application::data_path() + "image/iceland_heightmap.png").c_str(), &width,
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

            // vertex
            vertices.push_back(-height / 2.0f + height * i / (float) height);   // vx
            vertices.push_back((int) y * yScale - yShift);   // vy
            vertices.push_back(-width / 2.0f + width * j / (float) width);   // vz
        }
    }
    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
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
