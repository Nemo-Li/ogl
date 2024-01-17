//
// Created by Nemo li on 2024/1/16.
//

#ifndef OGL_QUAD_H
#define OGL_QUAD_H


#include "mesh.h"

class Quad {
public:
    Quad();

    Quad(float baseX, float baseY, float baseZ);

    Mesh *mesh() { return mesh_; }

    virtual ~Quad();

private:
    Mesh *mesh_;
    float baseX, baseY, baseZ;

    void initMesh();
};


#endif //OGL_QUAD_H
