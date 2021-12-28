//
// Created by Nemo li on 2021/12/28.
//

#ifndef OGL_FBOFILTER_H
#define OGL_FBOFILTER_H

#include "filter/IFilter.h"
#include "Shader.h"

class FboFilter : public Filter {
public:
    int vao;
    int texture;
    Shader *shader;
    int effect;

    void setEffect(int effect) override ;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};

#endif //OGL_FBOFILTER_H
