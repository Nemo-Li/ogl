//
// Created by Nemo li on 2024/1/19.
//

#ifndef OGL_INSTANCE_RENDER_H
#define OGL_INSTANCE_RENDER_H


#include "mesh_renderer.h"

class InstanceRender : public MeshRenderer {
public:
    InstanceRender();

    virtual ~InstanceRender();

    void Render() override;

RTTR_ENABLE(MeshRenderer);
};


#endif //OGL_INSTANCE_RENDER_H
