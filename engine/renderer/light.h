//
// Created by Nemo li on 2024/2/19.
//

#ifndef OGL_LIGHT_H
#define OGL_LIGHT_H

#include "component/component.h"

class Light : public Component {
public:
    void SetLightStrength(float lightStrength) { lightStrength_ = lightStrength; }

    float lightStrength() { return lightStrength_; }

private:
    float lightStrength_ = 0.1f;

RTTR_ENABLE();
};


#endif //OGL_LIGHT_H
