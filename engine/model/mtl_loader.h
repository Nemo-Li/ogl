//
// Created by Nemo li on 2024/1/11.
//

#ifndef OGL_MTL_LOADER_H
#define OGL_MTL_LOADER_H

#include <map>
#include <renderer/material.h>
#include <renderer/texture3d.h>

class MtlLoader {
public:
    Texture3D *texture3D;

    MtlLoader(const char *mtlPath);

    ~MtlLoader();

    std::map<std::string, Material *> obj_mtl_map_;

private:
    const char *mtlFilePath;
    int mtlCount;

    void readMtlInfo();

    void readMtlFile();
};


#endif //OGL_MTL_LOADER_H
