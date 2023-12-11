//
// Created by Nemo li on 2023/12/10.
//

#ifndef OGL_SKINNED_MESH_RENDERER_H
#define OGL_SKINNED_MESH_RENDERER_H

#include "mesh_renderer.h"

class SkinnedMeshRenderer : public MeshRenderer {
public:
    SkinnedMeshRenderer();

    virtual ~SkinnedMeshRenderer();

    void Update() override;

RTTR_ENABLE(MeshRenderer);
};

#endif //OGL_SKINNED_MESH_RENDERER_H
