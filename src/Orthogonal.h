//
// Created by Nemo li on 2021/12/11.
//

#ifndef OGL_ORTHOGONAL_H
#define OGL_ORTHOGONAL_H

#include "Model.h"
#include <glad/glad.h>
#include "Shader.h"
#include "OnProjectionListener.h"

class Orthogonal : Model, public OnProjectionListener {
public:
    unsigned int OrthogonalVAO;
    Shader *shader;
    glm::mat4 *modelMatrix;

    Orthogonal();

    Orthogonal(Shader *shader, glm::mat4 *modelMatrix);

    void initVAO(float left, float right, float top, float bottom, float near, float far);

    void draw() override;

    void onOrthoChange(float left, float right, float top, float bottom, float near, float far) override;
};


#endif //OGL_ORTHOGONAL_H
