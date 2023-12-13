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

    //加载一个图片文件
    static Texture2D *LoadFromFile(std::string &image_file_path);

    unsigned int texture_handle() { return texture_handle_; }

private:
    int width_;
    int height_;
    GLenum texture_format_;
    //纹理ID
    unsigned int texture_handle_;
};


#endif //OGL_TEXTURE2D_H
