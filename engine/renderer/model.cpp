//
// Created by Nemo li on 2024/1/11.
//

#include "model.h"
#include "framework/application.h"
#include "shader.h"
#include <map>

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

    mesh->vertex_data_ = new Vertex[indexCount];
    for (int i = 0; i < vertexCount; ++i) {
        Vertex vertex{};
        vertex.position_ = glm::vec3(loader->vArr[i][0], loader->vArr[i][1], loader->vArr[i][2]);
        vertex.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
        vertex.uv_ = glm::vec2();
        mesh->vertex_data_[i] = vertex;
    }

    mesh->vertex_index_data_ = new unsigned short[indexCount];
    mesh->textureIdBuffer = new short[indexCount];

    auto *texcoordMap = new std::map<int, bool>();
    texcoordMap->clear();
    int dupIndex = vertexCount;

    for (int i = 0; i < loader->faceCount; i++) {
        int index1 = loader->fvArr[i][0] - 1;
        int index2 = loader->fvArr[i][1] - 1;
        int index3 = loader->fvArr[i][2] - 1;

        glm::vec2 c1(loader->vtArr[loader->ftArr[i][0] - 1][0],
                     loader->vtArr[loader->ftArr[i][0] - 1][1]);
        glm::vec2 c2(loader->vtArr[loader->ftArr[i][1] - 1][0],
                     loader->vtArr[loader->ftArr[i][1] - 1][1]);
        glm::vec2 c3(loader->vtArr[loader->ftArr[i][2] - 1][0],
                     loader->vtArr[loader->ftArr[i][2] - 1][1]);

        // Duplicate vertex if texCoord not the same
        if (texcoordMap->find(index1) != texcoordMap->end() && mesh->vertex_data_[index1].uv_ != c1) {
            int newIndex = dupIndex++;
            Vertex vertex{};
            Vertex vertex1 = mesh->vertex_data_[index1];
            vertex.position_ = glm::vec3(vertex1.position_.x, vertex1.position_.y, vertex1.position_.z);
            vertex.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
            vertex.uv_ = glm::vec2();
            mesh->vertex_data_[newIndex] = vertex;
            index1 = newIndex;
        }

        if (texcoordMap->find(index2) != texcoordMap->end() && mesh->vertex_data_[index2].uv_ != c2) {
            int newIndex = dupIndex++;
            Vertex vertex{};
            Vertex vertex2 = mesh->vertex_data_[index2];
            vertex.position_ = glm::vec3(vertex2.position_.x, vertex2.position_.y, vertex2.position_.z);
            vertex.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
            vertex.uv_ = glm::vec2();
            mesh->vertex_data_[newIndex] = vertex;
            index2 = newIndex;
        }

        if (texcoordMap->find(index3) != texcoordMap->end() && mesh->vertex_data_[index3].uv_ != c3) {
            int newIndex = dupIndex++;
            Vertex vertex{};
            Vertex vertex3 = mesh->vertex_data_[index3];
            vertex.position_ = glm::vec3(vertex3.position_.x, vertex3.position_.y, vertex3.position_.z);
            vertex.color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
            vertex.uv_ = glm::vec2();
            mesh->vertex_data_[newIndex] = vertex;
            index3 = newIndex;
        }

        mesh->vertex_index_data_[i * 3] = index1;
        mesh->vertex_index_data_[i * 3 + 1] = index2;
        mesh->vertex_index_data_[i * 3 + 2] = index3;

        mesh->vertex_data_[index1].uv_ = c1;
        mesh->vertex_data_[index2].uv_ = c2;
        mesh->vertex_data_[index3].uv_ = c3;

        texcoordMap->insert(std::pair<int, bool>(index1, true));
        texcoordMap->insert(std::pair<int, bool>(index2, true));
        texcoordMap->insert(std::pair<int, bool>(index3, true));

        Material *mat = loader->mtlLoader->obj_mtl_map_[loader->mtArr[i]];
        if (mat) {
            mesh->textureIdBuffer[index1] = (short) mat->textureId.x;
            mesh->textureIdBuffer[index2] = (short) mat->textureId.x;
            mesh->textureIdBuffer[index3] = (short) mat->textureId.x;
        }
        mesh->vertex_num_ = dupIndex;
    }
}

Model::Model(const char *model_path, int vt) {
    std::string obj_file = Application::data_path() + model_path + ".obj";
    std::string mtl_file = Application::data_path() + model_path + ".mtl";
    printf("obj: %s mtl: %s\n", obj_file.c_str(), mtl_file.c_str());
    loadModelSimple(obj_file.c_str(), mtl_file.c_str(), vt);
}

Model::~Model() {
    printf("Model release\n");
    if (loader) {
        delete loader;
        loader = nullptr;
    }
    if (mesh) {
        delete mesh;
        mesh = nullptr;
    }
}

