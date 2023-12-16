//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MESH_RENDERER_H
#define OGL_MESH_RENDERER_H

#include <glm/glm.hpp>
#include "component/component.h"

class Material;

class MeshRenderer : public Component {
public:
    MeshRenderer();

    ~MeshRenderer();

    //设置Material
    void SetMaterial(Material *material);

    //渲染
    void Render();

    Material *material() { return material_; }

private:
    Material *material_;

    //顶点缓冲区对象句柄
    unsigned int vertex_buffer_object_handle_ = 0;
    //顶点数组对象句柄
    unsigned int vertex_array_object_handle_ = 0;

RTTR_ENABLE();
};


#endif //OGL_MESH_RENDERER_H
