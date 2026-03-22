//
// Created by Nemo li on 2024/3/4.
//

#ifndef OGL_EFFEKSEER_EFFECT_H
#define OGL_EFFEKSEER_EFFECT_H

#include "component/component.h"
#include "Effekseer/Effekseer.h"

class EffekseerEffect : public Component {
public:

    EffekseerEffect();

    void Render() override;

    void PlayEffect();

    void StopEffect();

    void OnDestroy() override;

private:
    Effekseer::Handle efkHandle = 0;
    Effekseer::ManagerRef efkManager;
    int32_t time = 0;
    Effekseer::EffectRef effect;
    ::Effekseer::Matrix44 projectionMatrix;
    ::Effekseer::Matrix44 cameraMatrix;
    ::Effekseer::Vector3D viewerPosition;

RTTR_ENABLE();
};


#endif //OGL_EFFEKSEER_EFFECT_H
