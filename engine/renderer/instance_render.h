//
// Created by Nemo li on 2024/1/19.
//

#ifndef OGL_INSTANCE_RENDER_H
#define OGL_INSTANCE_RENDER_H


#include <mesh/mesh.h>
#include "mesh_renderer.h"
#include "terrain.h"
#include "model.h"
#include <mesh/board.h>

class InstanceRender : public MeshRenderer {
public:
    InstanceRender();

    virtual ~InstanceRender();

    void Render() override;

    void InitInstance(Terrain *terrain);

private:
    //顶点数组object
    unsigned int vao_ = 0;
    Mesh *mesh = nullptr;
    Board *board = nullptr;
    unsigned int count = 4;
    float *position_ = nullptr;
    Model *tree_model = nullptr;

RTTR_ENABLE(MeshRenderer);
};


#endif //OGL_INSTANCE_RENDER_H
