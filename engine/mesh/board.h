//
// Created by Nemo li on 2024/1/19.
//

#ifndef OGL_BOARD_H
#define OGL_BOARD_H


#include "mesh.h"

class Board {
public:
    Board();

    Board(float baseX, float baseY, float baseZ);

    Board(float baseX, float baseY, float baseZ, float offX, float offY);

    Mesh *mesh() { return mesh_; }

    virtual ~Board();

private:
    Mesh *mesh_;
    float baseX, baseY, baseZ;
    float biasX, biasY;

    void initMesh();
};


#endif //OGL_BOARD_H
