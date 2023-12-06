//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MESH_FILTER_H
#define OGL_MESH_FILTER_H

#include <string>
#include <engine/component/component.h>
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
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
    };

    //Mesh数据
    struct Mesh {
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
        Vertex *vertex_data_;//顶点数据
        unsigned short *vertex_index_data_;//顶点索引数据
    };

    void LoadMesh(std::string mesh_file_path);//加载Mesh文件

    Mesh *mesh() { return mesh_; };//Mesh对象
private:
    Mesh *mesh_;//Mesh对象
};


#endif //OGL_MESH_FILTER_H
