//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MATERIAL_H
#define OGL_MATERIAL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "cube_map.h"

class Shader;

class Texture2D;

struct LightEffect {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Material {
public:

    Material();

    virtual ~Material();

    Shader *shader() { return shader_; }

    //加载Material文件并解析
    void Load(const std::string &material_path);

    std::vector<std::pair<std::string, Texture2D *>> &textures() { return textures_; }

    void SetCubeMap(CubeMap *cubeMap) { cubeMap_ = cubeMap; }

    CubeMap *GetCubeMap() { return cubeMap_; }

public:
    int id_;
    std::string name_;
    glm::vec4 textureId;
    Shader *shader_;
    LightEffect *lightEffect;
    std::vector<std::pair<std::string, Texture2D *>> textures_;

private:
    CubeMap *cubeMap_;
};


#endif //OGL_MATERIAL_H
