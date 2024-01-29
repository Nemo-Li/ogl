//
// Created by Nemo li on 2024/1/16.
//

#ifndef OGL_PLANE_H
#define OGL_PLANE_H


#include "mesh.h"

class Plane {
public:
    Plane();

    Plane(float baseX, float baseY, float baseZ);

    Mesh *mesh() { return mesh_; }

    virtual ~Plane();

private:
    Mesh *mesh_;
    float baseX, baseY, baseZ;

    void initMesh();
};


#endif //OGL_PLANE_H
