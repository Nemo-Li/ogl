//
// Created by Nemo li on 2024/2/29.
//

#ifndef OGL_ANIM_MESH_RENDER_H
#define OGL_ANIM_MESH_RENDER_H

#include <glm/glm.hpp>
#include "component/component.h"
#include "model/ass_animation.h"

class AssAnimation;

class Shader;

#define MAX_BONES 100
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class AnimMeshRender : public Component {
public:
    int curAnimation = 0;
    float animStartTime;
    bool playerActionEnd;
public:
    AnimMeshRender();

    //渲染
    void Render() override;

    void SetAnimation(AssAnimation *animation);

    void updateBoneTransform(Frame *frame);

private:
    unsigned int vao_;
    unsigned int *vbo_;
    unsigned int ebo_;
    std::vector<glm::mat4> Transforms;

    Shader *shader;
    AssAnimation *animation_;
    unsigned int m_boneLocation[MAX_BONES];
RTTR_ENABLE();
};


#endif //OGL_ANIM_MESH_RENDER_H
