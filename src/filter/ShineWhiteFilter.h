//
// Created by Nemo li on 2021/12/27.
//

#ifndef OGL_SHINEWHITEFILTER_H
#define OGL_SHINEWHITEFILTER_H

#include "IFilter.h"
#include "Shader.h"

class ShineWhiteFilter : public Filter {
public:
    int vao;
    int texture;
    Shader *shader;
    int mMaxFrames = 8;
    int mHalfFrames = mMaxFrames / 2;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};


#endif //OGL_SHINEWHITEFILTER_H
