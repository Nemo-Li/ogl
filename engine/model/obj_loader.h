//
// Created by Nemo li on 2024/1/11.
//

#ifndef OGL_OBJ_LOADER_H
#define OGL_OBJ_LOADER_H

#include <string>
#include "mtl_loader.h"

class ObjLoader {
private:
    const char *objFilePath;
    const char *mtlFilePath;
    int vtNumber;
    int vtCount, vnCount;

    void readObjInfo();

    void readObjFile();

public:
    int vCount, faceCount;
    float **vArr;
    float **vtArr;
    float **vnArr;
    int **fvArr;
    int **fnArr;
    int **ftArr;
    std::string *mtArr;
    MtlLoader *mtlLoader;

    ObjLoader(const char *objPath, const char *mtlPath, int vtNum);

    ~ObjLoader();
};


#endif //OGL_OBJ_LOADER_H
