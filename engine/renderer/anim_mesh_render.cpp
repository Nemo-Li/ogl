//
// Created by Nemo li on 2024/2/29.
//

#include "anim_mesh_render.h"
#include <rttr/registration>

RTTR_REGISTRATION {
    rttr::registration::class_<AnimMeshRender>("AnimMeshRender")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}


void AnimMeshRender::Render() {

}
