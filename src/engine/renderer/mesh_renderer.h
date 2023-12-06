//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MESH_RENDERER_H
#define OGL_MESH_RENDERER_H

#include <glm/glm.hpp>
#include "engine/component/component.h"

class Material;

class MeshRenderer : public Component {
public:
    MeshRenderer();

    ~MeshRenderer();

    //设置Material
    void SetMaterial(Material *material);

    void SetView(glm::mat4 view) { view_ = view; };

    void SetProjection(glm::mat4 projection) { projection_ = projection; };

    //渲染
    void Render();

private:
    Material *material_;

    glm::mat4 view_;
    glm::mat4 projection_;

    //顶点缓冲区对象
    unsigned int vbo_ = 0;
    //索引缓冲区对象
    unsigned int ebo_ = 0;
    //顶点数组对象
    unsigned int vao_ = 0;
};


#endif //OGL_MESH_RENDERER_H
