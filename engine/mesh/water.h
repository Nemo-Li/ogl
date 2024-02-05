//
// Created by Nemo li on 2024/2/4.
//

#ifndef OGL_WATER_H
#define OGL_WATER_H

#include "mesh.h"

class Water {
private:
    int waterSize;
    float waterHeight;
    Mesh *mesh;
private:
    void initFaces();

public:
    Water(int size, float height);

    Mesh *GetMesh() { return mesh; }

    virtual ~Water();
};


#endif //OGL_WATER_H
