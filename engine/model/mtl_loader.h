//
// Created by Nemo li on 2024/1/11.
//

#ifndef OGL_MTL_LOADER_H
#define OGL_MTL_LOADER_H

#include <map>

class MtlLoader {
public:
    MtlLoader(const char *mtlPath);

    ~MtlLoader();

    std::map<std::string, int> obj_mtl_map_;

private:
    const char *mtlFilePath;
    int mtlCount;

    void readMtlInfo();

    void readMtlFile();
};


#endif //OGL_MTL_LOADER_H
