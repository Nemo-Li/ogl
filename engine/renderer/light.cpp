//
// Created by Nemo li on 2024/2/19.
//

#include <glm/gtx/transform.hpp>
#include "light.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Light>("Light")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

glm::vec3 Light::stepRotate(float angle) {
    glm::mat4 rotateMat = glm::rotate(unit_mat4, glm::radians(angle), glm::vec3(1, 0, 0));
    lightDir_ = glm::vec3((rotateMat * glm::vec4(lightDir_, 1.0)));
    return lightDir_;
}
