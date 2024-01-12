//
// Created by Nemo li on 2024/1/12.
//

#include <framework/application.h>
#include <stb_image.h>
#include "texture3d.h"
#include "glad/glad.h"

using std::string;
using std::map;

Texture3D::Texture3D() {
    textureArrayId = 0;
    imageNames.clear();
    imageIdMap.clear();
}

Texture3D::~Texture3D() {
    releaseTexture3D();
    imageIdMap.clear();
    imageNames.clear();
}

void Texture3D::use(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
}

void Texture3D::addImage(const char *name) {
    //从0开始
    imageIdMap[name] = imageNames.size();
    imageNames.emplace_back(name);
}

int Texture3D::findImage(const char *name) {
    auto iter = imageIdMap.find(name);
    if (iter != imageIdMap.end())
        return iter->second;
    return -1;
}

void Texture3D::releaseTexture3D() {
    if (textureArrayId > 0)
        glDeleteTextures(1, &textureArrayId);
}

void Texture3D::initTexture3D(std::string &dir) {
    if (imageNames.empty()) return;

    glGenTextures(1, &textureArrayId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    string name;
    int width, height, components;
    for (unsigned int i = 0; i < imageNames.size(); i++) {
        name = imageNames[i];

        auto imageFile = std::string(Application::data_path()).append(dir).append(name);
        unsigned char *pixels = stbi_load(imageFile.c_str(), &width, &height,
                                          &components, 4);
        if (i == 0)
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
                         width, height,
                         imageNames.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                        width, height,
                        1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        stbi_image_free(pixels);
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
