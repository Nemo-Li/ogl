//
// Created by Nemo li on 2023/12/16.
//

#ifndef OGL_TEXTURE_H
#define OGL_TEXTURE_H

#include <string>
#include <glad/glad.h>

using std::string;

class Texture {
public:
    Texture();

    ~Texture();

    bool load(const std::string &file_name);

    void bind(int index = 0) const {
        if (to_id != 0) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, to_id);
        }
    }

    bool use_linear;

    GLuint to_id;
};

#endif //OGL_TEXTURE_H
