//
// Created by Nemo li on 2021/12/9.
//

#ifndef OGL_FRUSTUM_H
#define OGL_FRUSTUM_H

#include <glad/glad.h>
#include "math.h"
#include "Shader.h"
#include "Model.h"
#include "OnProjectionListener.h"

class Frustum : Model, public OnProjectionListener {
public:
    const float DEG2RAD = 3.1415926f / 180;
    unsigned int frustumVAO;
    Shader *shader;
    glm::mat4 *modelMatrix;

    Frustum();

    Frustum(Shader *shader, glm::mat4 *modelMatrix);

    void initFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane);

    void draw() override;

    void onPerspectiveChange(float fov, float near, float far, float width, float height) override;
};


#endif //OGL_FRUSTUM_H
