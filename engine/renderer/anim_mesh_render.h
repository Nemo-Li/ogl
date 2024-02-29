//
// Created by Nemo li on 2024/2/29.
//

#ifndef OGL_ANIM_MESH_RENDER_H
#define OGL_ANIM_MESH_RENDER_H

#include <glm/glm.hpp>
#include "component/component.h"

class AnimMeshRender : public Component {
public:
    //渲染
    virtual void Render();
private:

RTTR_ENABLE();
};


#endif //OGL_ANIM_MESH_RENDER_H
