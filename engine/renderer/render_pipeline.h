#ifndef OGL_RENDER_PIPELINE_H
#define OGL_RENDER_PIPELINE_H

#include <vector>
#include <memory>
#include "renderer/render_queue.h"
#include "renderer/camera.h"
#include "framework/scene.h"
#include "helpers/frustum.h"

class RenderPipeline {
public:
    virtual ~RenderPipeline() {}

    /// 执行渲染流程
    virtual void Render(Scene* scene) = 0;

protected:
    RenderQueue render_queue_;
    Frustum frustum_;
};

/// 默认的前向渲染管线
class ForwardRenderPipeline : public RenderPipeline {
public:
    void Render(Scene* scene) override {
        // 1. 遍历相机
        Camera::Foreach([&]() {
            auto* current_camera = Camera::current_camera();
            if (!current_camera) return;

            // 2. 清空并准备当前帧的渲染数据
            RenderQueue::Clear();
            frustum_.Update(current_camera->projection_mat4() * current_camera->view_mat4());

            // 3. 场景剔除与收集
            std::vector<GameObject*> visibleObjects;
            scene->GetVisibleObjects(frustum_, visibleObjects);

            // 4. 物体提交渲染命令
            for (auto* game_object : visibleObjects) {
                if (!game_object->active()) continue;
                
                game_object->ForeachComponent([](Component *component) {
                    component->Render();
                });
            }

            // 5. 执行渲染队列
            RenderQueue::Execute();
        });
    }
};

#endif //OGL_RENDER_PIPELINE_H
