//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MESH_FILTER_H
#define OGL_MESH_FILTER_H

#include <string>
#include <component/component.h>
#include <glm/glm.hpp>

class MeshFilter : public Component {
public:
    MeshFilter();

    ~MeshFilter();

public:
    //顶点
    struct Vertex {
        glm::vec3 position_;
        glm::vec4 color_;
        glm::vec2 uv_;
    };

    //Mesh文件头
    struct MeshFileHead {
        char type_[4];
        char name_[32];//名字
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
    };

    //Mesh数据
    struct Mesh {
        char *name_;//名字
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
        Vertex *vertex_data_;//顶点数据
        unsigned short *vertex_index_data_;//顶点索引数据

        Mesh() {
            name_ = nullptr;
            vertex_num_ = 0;
            vertex_index_num_ = 0;
            vertex_data_ = nullptr;
            vertex_index_data_ = nullptr;
        }

        ~Mesh() {
            if (vertex_data_ != nullptr) {
                delete[] vertex_data_;
                vertex_data_ = nullptr;
            }
            if (vertex_index_data_ != nullptr) {
                delete[] vertex_index_data_;
                vertex_index_data_ = nullptr;
            }
        }

        /// 获取字节数
        unsigned short size() {
            auto total_bytes_ = sizeof(vertex_num_) + vertex_num_ * sizeof(Vertex) + sizeof(vertex_index_num_) +
                                vertex_index_num_ * sizeof(unsigned short);
            return total_bytes_;
        }
    };

    /// 顶点关联骨骼及权重,每个顶点最多可以关联4个骨骼。
    struct VertexRelateBoneInfo {
        //骨骼索引，一般骨骼少于128个，用char就行。
        char bone_index_[4];
        //骨骼权重，权重不会超过100，所以用char类型就可以。
        char bone_weight_[4];
    };

    void LoadMesh(std::string mesh_file_path);//加载Mesh文件

    /// 创建 Mesh
    /// \param vertex_data 顶点数据
    /// \param vertex_index_data 索引数据
    void CreateMesh(std::vector<Vertex> &vertex_data, std::vector<unsigned short> &vertex_index_data);

    /// 创建Mesh
    /// \param vertex_data 所有的顶点数据,以float数组形式从lua传过来
    /// \param vertex_index_data 所有的索引数据,以unsigned short数组形式从lua传过来
    void CreateMesh(std::vector<float> &vertex_data, std::vector<unsigned short> &vertex_index_data);

    Mesh *mesh() { return mesh_; };//Mesh对象

    /// 获取Mesh名
    const char *GetMeshName();

    /// 获取顶点关联骨骼索引数组，长度为顶点个数
    std::vector<unsigned char> &vertex_relate_bone_index_vec() { return vertex_relate_bone_index_vec_; };

    void set_vertex_relate_bone_index_vec(std::vector<unsigned char> &vertex_relate_bone_index_vec) {
        vertex_relate_bone_index_vec_ = vertex_relate_bone_index_vec;
    }

    /// 获取蒙皮Mesh对象指针
    Mesh *skinned_mesh() { return skinned_mesh_; };

    void set_skinned_mesh(Mesh *skinned_mesh) { skinned_mesh_ = skinned_mesh; };

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

private:
    //Mesh对象
    Mesh *mesh_;
    //蒙皮Mesh对象
    Mesh *skinned_mesh_ = nullptr;
    //顶点关联骨骼索引，长度为顶点个数
    std::vector<unsigned char> vertex_relate_bone_index_vec_;
    //顶点关联骨骼信息(4个骨骼索引、权重)，长度为顶点数
    VertexRelateBoneInfo *vertex_relate_bone_infos_ = nullptr;

RTTR_ENABLE();
};


#endif //OGL_MESH_FILTER_H
