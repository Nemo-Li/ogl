//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_MATERIAL_H
#define OGL_MATERIAL_H

#include <string>
#include <vector>

class Shader;

class Texture2D;

class Material {
public:
    Material();

    virtual ~Material();

    Shader *shader() { return shader_; }

    //加载Material文件并解析
    void Load(const std::string &material_path);

    std::vector<std::pair<std::string, Texture2D *>> &textures() { return textures_; }

    void SetUniformMatrix4fv(const std::string &shader_property_name, float *pointer);

    void SetUniform1i(const std::string &shader_property_name, int value);

private:
    Shader *shader_;

    std::vector<std::pair<std::string, Texture2D *>> textures_;
};


#endif //OGL_MATERIAL_H
