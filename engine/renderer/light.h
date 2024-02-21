//
// Created by Nemo li on 2024/2/19.
//

#ifndef OGL_LIGHT_H
#define OGL_LIGHT_H

#include <glm/glm.hpp>
#include "component/component.h"

class Light : public Component {
public:
    void SetLightStrength(float lightStrength) { lightStrength_ = lightStrength; }

    float lightStrength() { return lightStrength_; }

    glm::vec3 lightDir() { return lightDir_; }

    glm::vec3 stepRotate(float angle);

private:
    float lightStrength_ = 1.0f;

    glm::vec3 lightDir_ = glm::vec3(0, 1, 0.2f);

    glm::mat4 unit_mat4 = glm::mat4(1.0f);

RTTR_ENABLE();
};


#endif //OGL_LIGHT_H
