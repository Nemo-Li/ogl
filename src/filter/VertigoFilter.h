//
// Created by Nemo li on 2021/12/27.
//

#ifndef OGL_VERTIGOFILTER_H
#define OGL_VERTIGOFILTER_H

#include "IFilter.h"
#include "Shader.h"
#include "fbo/FrameBuffer.h"
#include "Texture.h"

class VertigoFilter : public Filter {
public:
    int vao;
    unsigned int texture;
    Shader *currentShader;
    Shader *lastShader;
    FrameBuffer *frameBuffer1;
    FrameBuffer *frameBuffer2;
    FrameBuffer *frameBuffer3;
    bool mFirst = true;
    Texture lutTexture = Texture();

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;

    void drawToBuffer();

    void drawCurrentFrame();

    void setup(Shader *shader, unsigned int textureId[], int size);

    virtual ~VertigoFilter();
};


#endif //OGL_VERTIGOFILTER_H
