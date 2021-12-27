//
// Created by Nemo li on 2021/12/26.
//

#ifndef OGL_JITTERFILTER_H
#define OGL_JITTERFILTER_H

#include "IFilter.h"
#include "Shader.h"

class JitterFilter : public Filter {

public:
    int vao;
    int texture;
    Shader *shader;
    //动画最大帧数
    int mMaxFrames = 8;
    //动画完成后跳过的帧数
    int mSkipFrames = 4;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};


#endif //OGL_JITTERFILTER_H
