//
// Created by Nemo li on 2024/1/11.
//

#ifndef OGL_MODEL_H
#define OGL_MODEL_H

#include <iostream>
#include <vector>
#include "model/obj_loader.h"
#include "mesh.h"
#include "texture3d.h"

class Model {
public:
    Model(const char *model_path, int vt);

    Model(const char *obj, const char *mtl, int vt);

    virtual ~Model();

    void loadModelSimple(const char *obj, const char *mtl, int vt);

    Mesh *GetMesh() { return mesh; }

public:
    Texture3D *texture3D = nullptr;
    Shader *shader = nullptr;

private:
    Mesh *mesh = nullptr;

    ObjLoader *loader = nullptr;

    void initFacesWidthIndices();
};


#endif //OGL_MODEL_H
