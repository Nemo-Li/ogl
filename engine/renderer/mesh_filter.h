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
        glm::vec3 pos_;
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

    void LoadMesh(std::string mesh_file_path);//加载Mesh文件

    Mesh *mesh() { return mesh_; };//Mesh对象

    /// 获取Mesh名
    const char *GetMeshName();

private:
    Mesh *mesh_;//Mesh对象

RTTR_ENABLE();
};


#endif //OGL_MESH_FILTER_H
