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

    void readObjInfo();

    void readObjFile();

public:
    int vCount = 0, faceCount = 0;
    int vtCount, vnCount;
    //材质数量 不同材质mesh 不同渲染
    int mtCount = 0;
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
