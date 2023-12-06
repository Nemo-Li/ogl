//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MATERIAL_H
#define OGL_MATERIAL_H

#include <string>

class Shader;

class Material {
public:
    Material();

    virtual ~Material();

    Shader *shader() { return shader_; }

    //加载Material文件并解析
    void Load(std::string material_path);

private:
    Shader *shader_;
};


#endif //OGL_MATERIAL_H
