//
// Created by Nemo li on 2023/12/6.
//

#include "mesh_filter.h"
#include <fstream>
#include <rttr/registration>
#include <framework/application.h>

using std::ifstream;
using std::ios;

RTTR_REGISTRATION {
    rttr::registration::class_<MeshFilter>("MeshFilter")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

MeshFilter::MeshFilter() : mesh_(nullptr) {

}

MeshFilter::~MeshFilter() {
    delete (mesh_);
    mesh_ = nullptr;
}

void MeshFilter::LoadMesh(std::string mesh_file_path) {
    //读取 Mesh文件头
    ifstream input_file_stream(Application::data_path() + mesh_file_path, ios::in | ios::binary);
    MeshFileHead mesh_file_head;
    input_file_stream.read((char *) &mesh_file_head, sizeof(mesh_file_head));
    //读取顶点数据
    unsigned char *vertex_data = (unsigned char *) malloc(mesh_file_head.vertex_num_ * sizeof(Vertex));
    input_file_stream.read((char *) vertex_data, mesh_file_head.vertex_num_ * sizeof(Vertex));
    //读取顶点索引数据
    unsigned short *vertex_index_data = (unsigned short *) malloc(
            mesh_file_head.vertex_index_num_ * sizeof(unsigned short));
    input_file_stream.read((char *) vertex_index_data, mesh_file_head.vertex_index_num_ * sizeof(unsigned short));
    input_file_stream.close();

    mesh_ = new Mesh();
    mesh_->vertex_num_ = mesh_file_head.vertex_num_;
    mesh_->vertex_index_num_ = mesh_file_head.vertex_index_num_;
    mesh_->vertex_data_ = (Vertex *) vertex_data;
    mesh_->vertex_index_data_ = vertex_index_data;
}

const char *MeshFilter::GetMeshName() {
    return mesh_->name_;
}

void MeshFilter::CreateMesh(std::vector<Vertex> &vertex_data, std::vector<unsigned short> &vertex_index_data) {
    if (mesh_ != nullptr) {
        delete mesh_;
        mesh_ = nullptr;
    }
    mesh_ = new Mesh();
    mesh_->vertex_num_ = vertex_data.size();
    mesh_->vertex_index_num_ = vertex_index_data.size();

    unsigned short vertex_data_size = mesh_->vertex_num_ * sizeof(Vertex);
    mesh_->vertex_data_ = static_cast<Vertex *>(malloc(vertex_data_size));
    memcpy(mesh_->vertex_data_, &vertex_data[0], vertex_data_size);

    unsigned short vertex_index_data_size = mesh_->vertex_num_ * sizeof(Vertex);
    mesh_->vertex_index_data_ = static_cast<unsigned short *>(malloc(vertex_index_data_size));
    memcpy(mesh_->vertex_index_data_, &vertex_index_data[0], vertex_index_data_size);
}

void MeshFilter::CreateMesh(std::vector<float> &vertex_data, std::vector<unsigned short> &vertex_index_data) {
    if (mesh_ != nullptr) {
        delete mesh_;
        mesh_ = nullptr;
    }
    mesh_ = new Mesh();
    mesh_->vertex_num_ = vertex_data.size() / 9;//一个vertex由9个float组成。
    mesh_->vertex_index_num_ = vertex_index_data.size();

    unsigned short vertex_data_size = mesh_->vertex_num_ * sizeof(Vertex);
    mesh_->vertex_data_ = static_cast<Vertex *>(malloc(vertex_data_size));
    memcpy(mesh_->vertex_data_, &vertex_data[0], vertex_data_size);

    unsigned short vertex_index_data_size = mesh_->vertex_index_num_ * sizeof(unsigned short);
    mesh_->vertex_index_data_ = static_cast<unsigned short *>(malloc(vertex_index_data_size));
    memcpy(mesh_->vertex_index_data_, &vertex_index_data[0], vertex_index_data_size);
}

void MeshFilter::LoadWeight(const std::string &weight_file_path) {
    ifstream input_weight_file(Application::data_path() + weight_file_path, ios::in | ios::binary);
    if (!input_weight_file.is_open()) {
        DEBUG_LOG_ERROR("weight file open failed");
        return;
    }
    char file_head[7];
    input_weight_file.read(file_head, 6);
    file_head[6] = '\0';
    if (strcmp(file_head, "weight") != 0) {
        DEBUG_LOG_ERROR("weight file head error");
        return;
    }
    input_weight_file.seekg(0, ios::end);
    int length = input_weight_file.tellg();
    input_weight_file.seekg(6, ios::beg);
    vertex_relate_bone_infos_ = (VertexRelateBoneInfo *) malloc(length - 6);
    input_weight_file.read((char *) vertex_relate_bone_infos_, length - 6);
    input_weight_file.close();
}
