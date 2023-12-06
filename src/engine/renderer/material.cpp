//
// Created by Nemo li on 2023/12/6.
//

#include "material.h"
#include "shader.h"

Material::Material() {}

Material::~Material() {

}

void Material::Load(std::string material_path) {
    shader_ = Shader::Find("shaders/Unlit");
}
