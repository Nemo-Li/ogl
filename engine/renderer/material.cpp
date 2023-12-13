//
// Created by Nemo li on 2023/12/6.
//

#include <app/application.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "material.h"
#include "shader.h"
#include "texture2d.h"

Material::Material() {

}

Material::~Material() {
}

void Material::Load(const std::string &material_path) {
    //解析xml
    rapidxml::file<> xml_file((Application::data_path() + material_path).c_str());
    rapidxml::xml_document<> document;
    document.parse<0>(xml_file.data());

    //根节点
    rapidxml::xml_node<> *material_node = document.first_node("material");
    if (material_node == nullptr) {
        return;
    }

    rapidxml::xml_attribute<> *material_shader_attribute = material_node->first_attribute("shader");
    if (material_shader_attribute == nullptr) {
        return;
    }

    shader_ = Shader::Find(material_shader_attribute->value());

    //解析Texture
    rapidxml::xml_node<> *material_texture_node = material_node->first_node("texture");
    while (material_texture_node != nullptr) {
        rapidxml::xml_attribute<> *texture_name_attribute = material_texture_node->first_attribute("name");
        if (texture_name_attribute == nullptr) {
            return;
        }

        rapidxml::xml_attribute<> *texture_image_attribute = material_texture_node->first_attribute("image");
        if (texture_image_attribute == nullptr) {
            return;
        }

        std::string image_path = texture_image_attribute->value();
        textures_.emplace_back(texture_name_attribute->value(),
                               image_path.empty() ? nullptr : Texture2D::LoadFromFile(image_path));

        material_texture_node = material_texture_node->next_sibling("texture");
    }
}

void Material::SetUniformMatrix4fv(const std::string &shader_property_name, float *pointer) {

}

void Material::SetUniform1i(const std::string &shader_property_name, int value) {

}
