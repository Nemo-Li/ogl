//
// Created by Nemo li on 2021/12/11.
//

#ifndef OGL_ORTHOGONAL_H
#define OGL_ORTHOGONAL_H

#include "Model.h"
#include <glad/glad.h>
#include "Shader.h"

class Orthogonal : Model {
public:
    unsigned int OrthogonalVAO;
    Shader *shader;
    glm::mat4 *modelMatrix;

    Orthogonal();

    Orthogonal(Shader *shader, glm::mat4 *modelMatrix);

    void initVAO(float nearPlane, float farPlane, float width, float height);

    void draw() override;
};


#endif //OGL_ORTHOGONAL_H
