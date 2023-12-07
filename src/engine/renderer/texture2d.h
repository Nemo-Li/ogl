//
// Created by Nemo li on 2023/12/7.
//

#ifndef OGL_TEXTURE2D_H
#define OGL_TEXTURE2D_H


#include <string>
#include <glad/glad.h>

class Texture2D {
public:
    Texture2D();

    virtual ~Texture2D();

    void bind(int index = 0) const {
        if (texture_id_ != 0) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, texture_id_);
        }
    }

    //加载一个图片文件
    static Texture2D *LoadFromFile(std::string &image_file_path);

    GLuint texture_id() { return texture_id_; }

private:
    int width_;
    int height_;
    GLenum texture_format_;
    //纹理ID
    GLuint texture_id_;
};


#endif //OGL_TEXTURE2D_H
