//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_RENDER_TASK_CONSUMER_EDITOR_H
#define OGL_RENDER_TASK_CONSUMER_EDITOR_H

#include "render_task_consumer_base.h"
#include "glad/glad.h"

struct GLFWwindow;

class RenderTaskBase;

/// 渲染任务消费端(编辑器)
class RenderTaskConsumerEditor : public RenderTaskConsumerBase {
public:
    RenderTaskConsumerEditor(GLFWwindow *window);

    ~RenderTaskConsumerEditor() override;

    void InitGraphicsLibraryFramework() override;

    void GetFramebufferSize(int &width, int &height) override;

    void SwapBuffer() override;

    /// FBO 颜色纹理
    /// \return
    GLuint &color_texture_id() {
        return color_texture_id_;
    }

    /// FBO 深度纹理
    /// \return
    GLuint &depth_texture_id() {
        return depth_texture_id_;
    }

private:
    GLFWwindow *window_ = nullptr;
    //FBO 颜色纹理
    GLuint color_texture_id_ = 0;
    //FBO 深度纹理
    GLuint depth_texture_id_ = 0;
};


#endif //OGL_RENDER_TASK_CONSUMER_EDITOR_H
