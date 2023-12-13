//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_RENDER_TASK_CONSUMER_GAME_H
#define OGL_RENDER_TASK_CONSUMER_GAME_H

#include <thread>
#include "render_async/render_task_consumer_base.h"

class GLFWwindow;

class RenderTaskBase;

class RenderTaskConsumerGame : public RenderTaskConsumerBase {
public:
    RenderTaskConsumerGame(GLFWwindow *window);

    ~RenderTaskConsumerGame() override;

    void InitGraphicsLibraryFramework() override;

    void GetFramebufferSize(int &width, int &height) override;

    void SwapBuffer() override;

private:
    GLFWwindow *window_ = nullptr;

};


#endif //OGL_RENDER_TASK_CONSUMER_GAME_H
