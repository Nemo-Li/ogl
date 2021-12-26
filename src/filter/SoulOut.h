//
// Created by Nemo li on 2021/12/26.
//

#ifndef OGL_SOULOUT_H
#define OGL_SOULOUT_H

#include "IFilter.h"
#include "glad/glad.h"

class SoulOut : public Filter {
public:
    Shader *shader;
    int vao;
    int texture;

    void drawFilter() override;

    void init() override;

    void setVAOTexture(int vao, int texture) override;
};


#endif //OGL_SOULOUT_H
