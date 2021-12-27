//
// Created by Nemo li on 2021/12/26.
//

#ifndef OGL_SOULOUT_H
#define OGL_SOULOUT_H

#include "IFilter.h"
#include "glad/glad.h"

class SoulOut : public Filter {
public:
    //动画最大帧数
    int mMaxFrames = 15;
    //动画完成后跳过的帧数
    int mSkipFrames = 8;

    Shader *shader;
    int vao;
    int texture;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};


#endif //OGL_SOULOUT_H
