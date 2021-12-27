//
// Created by Nemo li on 2021/12/27.
//

#include <iostream>
#include "FrameBuffer.h"

unsigned int FrameBuffer::getTextureId() {
    return textureId;
}

void FrameBuffer::init(int activeTextureUnit, int width, int height) {
    this->mWidth = width;
    this->mHeight = height;

    GLuint buffer;

    // Generate frame buffer
    glGenFramebuffers(1, &buffer);
    mFrameBufferId = buffer;
    // Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);

    glActiveTexture(activeTextureUnit);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Generate render buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textureId, 0);
    checkGlError("glFramebufferTexture2D");

    glGenRenderbuffers(1, &buffer);
    mRenderBufferId = buffer;
    // Bind render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, mRenderBufferId);
    checkGlError("glFramebufferRenderbuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGlError("unbind");
}

void FrameBuffer::bind() {
//    glViewport(0, 0, mWidth / 2, mHeight / 2);
//    checkGlError("glViewport");
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
    checkGlError("glBindFramebuffer");
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::checkGlError(char *op) {
    int error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << op << ": glError 0x" << error << std::endl;
    }
}

