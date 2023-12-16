//
// Created by Nemo li on 2023/12/16.
//

#ifndef OGL_CONTROL_H
#define OGL_CONTROL_H

#include <glm/glm.hpp>

void computeMatricesFromInputs();

glm::mat4 getViewMatrix();

glm::mat4 getProjectionMatrix();

#endif //OGL_CONTROL_H
