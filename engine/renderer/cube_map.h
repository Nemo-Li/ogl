//
// Created by Nemo li on 2024/1/6.
//

#ifndef OGL_CUBE_MAP_H
#define OGL_CUBE_MAP_H

#include <string>
#include <vector>

class CubeMap {
public:
    unsigned int textureID;

    unsigned int loadCubeMap(std::vector<std::string> &faces);
};

#endif //OGL_CUBE_MAP_H
