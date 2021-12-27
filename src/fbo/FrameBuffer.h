//
// Created by Nemo li on 2021/12/27.
//

#ifndef OGL_FRAMEBUFFER_H
#define OGL_FRAMEBUFFER_H

#include "glad/glad.h"

class FrameBuffer {
public:
    unsigned int textureId;
    unsigned int mRenderBufferId;
    unsigned int mFrameBufferId;
    int mWidth, mHeight;

    void checkGlError(char *op);

    unsigned int getTextureId();

    void init(int activeTextureUnit, int width, int height);

    void bind();

    void unbind();
};


#endif //OGL_FRAMEBUFFER_H
