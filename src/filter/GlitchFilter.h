//
// Created by Nemo li on 2021/12/27.
//

#ifndef OGL_GLITCHFILTER_H
#define OGL_GLITCHFILTER_H

#include "IFilter.h"
#include "Shader.h"

class GlitchFilter : public Filter {
public:
    int vao;
    int texture;
    Shader *shader;
    int mMaxFrames = 8;
    float mDriftSequence[9] = {0.0f, 0.03f, 0.032f, 0.035f, 0.03f, 0.032f, 0.031f, 0.029f, 0.025f};
    float mJitterSequence[9] = {0.0f, 0.03f, 0.01f, 0.02f, 0.05f, 0.055f, 0.03f, 0.02f, 0.025f};
    float mThreshHoldSequence[9] = {1.0f, 0.965f, 0.9f, 0.9f, 0.9f, 0.6f, 0.8f, 0.5f, 0.5f};

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;

};


#endif //OGL_GLITCHFILTER_H
