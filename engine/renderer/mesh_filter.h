//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MESH_FILTER_H
#define OGL_MESH_FILTER_H

#include <string>
#include <component/component.h>
#include <glm/glm.hpp>
#include "renderer/model.h"
#include "mesh.h"

#define MESH 0
#define SKIN_MESH 1
#define MODEL 2

class MeshFilter : public Component {
public:
    MeshFilter();

    ~MeshFilter();

public:
    /// 顶点关联骨骼及权重,每个顶点最多可以关联4个骨骼。
    struct VertexRelateBoneInfo {
        //骨骼索引，一般骨骼少于128个，用char就行。
        char bone_index_[4];
        //骨骼权重，权重不会超过100，所以用char类型就可以。
        char bone_weight_[4];
    };

    //加载Mesh文件
    void LoadMesh(std::string mesh_file_path);

    /// 创建 Mesh
    /// \param vertex_data 顶点数据
    /// \param vertex_index_data 索引数据
    void CreateMesh(std::vector<Vertex> &vertex_data, std::vector<unsigned short> &vertex_index_data);

    /// 创建Mesh
    /// \param vertex_data 所有的顶点数据,以float数组形式从lua传过来
    /// \param vertex_index_data 所有的索引数据,以unsigned short数组形式从lua传过来
    void CreateMesh(std::vector<float> &vertex_data, std::vector<unsigned short> &vertex_index_data);

    //Mesh对象
    Mesh *mesh() { return mesh_; };

    /// 获取Mesh名
    const char *GetMeshName();

    /// 获取顶点关联骨骼索引数组，长度为顶点个数
    std::vector<unsigned char> &vertex_relate_bone_index_vec() { return vertex_relate_bone_index_vec_; };

    void set_vertex_relate_bone_index_vec(std::vector<unsigned char> &vertex_relate_bone_index_vec) {
        vertex_relate_bone_index_vec_ = vertex_relate_bone_index_vec;
    }

    /// 获取蒙皮Mesh对象指针
    Mesh *skinned_mesh() { return skinned_mesh_; };

    void set_skinned_mesh(Mesh *skinned_mesh) {
        mesh_filter_type_ = SKIN_MESH;
        skinned_mesh_ = skinned_mesh;
    };

    /// 获取顶点关联骨骼信息(4个骨骼索引、骨骼权重)，长度为顶点个数
    VertexRelateBoneInfo *vertex_relate_bone_infos() { return vertex_relate_bone_infos_; };


    /// 设置顶点关联骨骼信息
    /// \param vertex_relate_bone_info_data unsigned char数组形式，长度为顶点个数*8.
    /// 每个顶点按照 bone_index_[4] bone_weight_[4] 的顺序存储，
    void set_vertex_relate_bone_infos(std::vector<char> &vertex_relate_bone_info_data) {
        if (vertex_relate_bone_infos_ != nullptr) {
            delete[] vertex_relate_bone_infos_;
            vertex_relate_bone_infos_ = nullptr;
        }
        size_t data_size = vertex_relate_bone_info_data.size() * sizeof(char);
        vertex_relate_bone_infos_ = static_cast<VertexRelateBoneInfo *>(malloc(data_size));
        for (int i = 0; i < data_size; ++i) {
            ((char *) vertex_relate_bone_infos_)[i] = vertex_relate_bone_info_data[i];
        }
    }

    /// 加载权重文件
    /// \param weight_file_path 权重文件路径
    void LoadWeight(const std::string &weight_file_path);

    //加载Mesh文件
    void SetModel(Model *model) {
        mesh_filter_type_ = MODEL;
        model_ = model;
    }

    Model *GetModel() {
        return model_;
    }

    int GetMeshFilterType() { return mesh_filter_type_; }

    void SetMeshFilterType(int type) { mesh_filter_type_ = type; }

    void OnDestroy() override;

private:
    //mesh类型
    int mesh_filter_type_ = MESH;
    //Mesh对象
    Mesh *mesh_ = nullptr;
    //蒙皮Mesh对象
    Mesh *skinned_mesh_ = nullptr;
    //obj 文件加载
    Model *model_ = nullptr;
    //顶点关联骨骼索引，长度为顶点个数
    std::vector<unsigned char> vertex_relate_bone_index_vec_;
    //顶点关联骨骼信息(4个骨骼索引、权重)，长度为顶点数
    VertexRelateBoneInfo *vertex_relate_bone_infos_ = nullptr;

RTTR_ENABLE();
};


#endif //OGL_MESH_FILTER_H
