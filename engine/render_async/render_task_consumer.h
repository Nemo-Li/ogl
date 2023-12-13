//
// Created by Nemo li on 2023/12/12.
//

#ifndef OGL_RENDER_TASK_CONSUMER_H
#define OGL_RENDER_TASK_CONSUMER_H

#include <thread>

class GLFWwindow;

class RenderTaskBase;

class RenderTaskConsumerBase;

/// 渲染任务消费端
class RenderTaskConsumer {
public:
    static void Init(RenderTaskConsumerBase *instance);

    static RenderTaskConsumerBase *Instance();

    static void Exit();

private:
    static RenderTaskConsumerBase *instance_;
};


#endif //OGL_RENDER_TASK_CONSUMER_H
