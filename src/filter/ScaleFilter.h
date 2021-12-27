//
// Created by Nemo li on 2021/12/27.
//

#ifndef OGL_SCALEFILTER_H
#define OGL_SCALEFILTER_H

#include "IFilter.h"
#include "Shader.h"

class ScaleFilter : public Filter {
public:
    int vao;
    int texture;
    Shader *shader;
    //最大缩放是1.3倍
    float mScale = 0.3f;
    //最大帧数是14帧，通过这个控制动画速度
    int mMaxFrames = 14;
    int mMiddleFrames = mMaxFrames / 2;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};


#endif //OGL_SCALEFILTER_H
