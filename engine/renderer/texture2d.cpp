//
// Created by Nemo li on 2023/12/7.
//

#include <stb_image.h>
#include <framework/application.h>
#include "texture2d.h"

Texture2D::Texture2D() {

}

Texture2D::~Texture2D() {
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}

Texture2D *Texture2D::LoadFromFile(std::string &image_file_path) {
    Texture2D *texture2d = new Texture2D();
    int width, height, components;

    printf("image path: %s\n", image_file_path.c_str());
    stbi_set_flip_vertically_on_load(true);
    unsigned char *pixels = stbi_load((Application::data_path() + image_file_path).c_str(), &width, &height,
                                      &components, 4);

    if (pixels != nullptr) {
        //1. 通知显卡创建纹理对象，返回句柄;
        glGenTextures(1, &texture2d->texture_id_);
        //2. 将纹理绑定到特定纹理目标;
        glBindTexture(GL_TEXTURE_2D, texture2d->texture_id_);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glTexSubImage2D(GL_TEXTURE_2D, 0 /* mip map level */, 0 /* x offset */, 0 /* y offset */, width, height,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        printf("Could not load file %s\n", image_file_path.c_str());
    }
    stbi_image_free(pixels);
    return texture2d;
}
