//
// Created by Nemo li on 2023/12/16.
//

#include "Texture.h"
#include "stb_image.h"
#include "iostream"

using std::cout;

Texture::Texture()
        : use_linear(true), to_id(0) {
}

Texture::~Texture() {
    if (to_id != 0) {
        glDeleteTextures(1, &to_id);
        to_id = 0;
    }
}

bool Texture::load(const std::string &file_name) {
    if (file_name.empty()) {
        return false;
    }

    bool is_loaded = false;
    int width, height, components;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *pixels = stbi_load(("../../../res/" + file_name).c_str(), &width, &height, &components, 4);

    if (pixels != nullptr) {
        GLenum format = GL_RGBA;
        if (components == 1)
            format = GL_RED;
        else if (components == 3)
            format = GL_RGB;
        else if (components == 4)
            format = GL_RGBA;

        glGenTextures(1, &to_id);
        glBindTexture(GL_TEXTURE_2D, to_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        std::cout << "Could not load file " << file_name << std::endl;
    }

    stbi_image_free(pixels);
    return is_loaded;
}