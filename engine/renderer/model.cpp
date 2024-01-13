//
// Created by Nemo li on 2024/1/11.
//

#include "model.h"
#include "framework/application.h"
#include "shader.h"

Model::Model(const char *obj, const char *mtl, int vt) {
    loadModelSimple(obj, mtl, vt);
}

void Model::loadModelSimple(const char *obj, const char *mtl, int vt) {
    loader = new ObjLoader(obj, mtl, vt);
    texture3D = loader->mtlLoader->texture3D;
    initFacesWidthIndices();
}

void Model::initFacesWidthIndices() {
    printf("mt count %d \n", loader->mtCount);
    mesh = new Mesh();
    mesh->name_ = std::string("sponza").c_str();
    int vertexCount = loader->vCount;
    mesh->vertex_num_ = vertexCount;
    int indexCount = loader->faceCount * 3;
    mesh->vertex_index_num_ = indexCount;

    mesh->vertex_data_ = new Vertex[vertexCount];
    for (int i = 0; i < vertexCount; ++i) {
        Vertex vertex{};
        vertex.position_ = glm::vec3(loader->vArr[i][0], loader->vArr[i][1], loader->vArr[i][2]);
        vertex.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
        vertex.uv_ = glm::vec2();
        mesh->vertex_data_[i] = vertex;
    }

    mesh->vertex_index_data_ = new unsigned short[indexCount];
    mesh->material_ = new Material *[vertexCount];
    mesh->textureIdBuffer = new short[vertexCount];
    for (int i = 0; i < loader->faceCount; i++) {
        int index1 = loader->fvArr[i][0] - 1;
        int index2 = loader->fvArr[i][1] - 1;
        int index3 = loader->fvArr[i][2] - 1;

        mesh->vertex_index_data_[i * 3] = index1;
        mesh->vertex_index_data_[i * 3 + 1] = index2;
        mesh->vertex_index_data_[i * 3 + 2] = index3;

        glm::vec2 c1(loader->vtArr[loader->ftArr[i][0] - 1][0],
                     loader->vtArr[loader->ftArr[i][0] - 1][1]);
        glm::vec2 c2(loader->vtArr[loader->ftArr[i][1] - 1][0],
                     loader->vtArr[loader->ftArr[i][1] - 1][1]);
        glm::vec2 c3(loader->vtArr[loader->ftArr[i][2] - 1][0],
                     loader->vtArr[loader->ftArr[i][2] - 1][1]);
        mesh->vertex_data_[index1].uv_ = c1;
        mesh->vertex_data_[index2].uv_ = c2;
        mesh->vertex_data_[index3].uv_ = c3;

        Material *mat = loader->mtlLoader->obj_mtl_map_[loader->mtArr[i]];
        mesh->material_[index1] = mat;
        mesh->material_[index2] = mat;
        mesh->material_[index3] = mat;

        mesh->textureIdBuffer[index1] = (short) mat->textureId.x;
//        printf("index1 %d\n", index1);
//        mesh->textureIdBuffer[index1 * 4 + 1] = mat->textureId.y;
//        mesh->textureIdBuffer[index1 * 4 + 2] = mat->textureId.z;
//        mesh->textureIdBuffer[index1 * 4 + 3] = mat->textureId.w;
    }
}

Model::Model(const char *model_path, int vt) {
    std::string obj_file = Application::data_path() + model_path + ".obj";
    std::string mtl_file = Application::data_path() + model_path + ".mtl";
    printf("obj: %s mtl: %s\n", obj_file.c_str(), mtl_file.c_str());
    loadModelSimple(obj_file.c_str(), mtl_file.c_str(), vt);
}

Model::~Model() {
    if (loader) {
        delete loader;
        loader = nullptr;
    }
    if (mesh) {
        delete mesh;
        mesh = nullptr;
    }
}

