//
// Created by Nemo li on 2021/12/9.
//

#ifndef OGL_FRUSTUM_H
#define OGL_FRUSTUM_H

#include <glad/glad.h>
#include "math.h"
#include "Shader.h"
#include "Model.h"

class Frustum : Model{
public:
    const float DEG2RAD = 3.1415926f / 180;
    unsigned int frustumVAO;
    Shader *shader;
    glm::mat4 *modelMatrix;

    Frustum();

    Frustum(Shader *shader, glm::mat4 *modelMatrix);

    void initFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane);

    void draw();
};


#endif //OGL_FRUSTUM_H
