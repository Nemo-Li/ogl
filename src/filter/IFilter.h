//
// Created by Nemo li on 2021/12/26.
//

#ifndef OGL_IFILTER_H
#define OGL_IFILTER_H

#include "Shader.h"

class Filter {
public:
    virtual void drawFilter() = 0;

    virtual void init() = 0;

    virtual void setVAOTexture(int vao, int texture) = 0;

    virtual void setEffect(int effect) {

    };

    //当前动画进度
    float mProgress = 0.0f;
    //当前地帧数
    int mFrames = 0;

};

#endif //OGL_IFILTER_H
