#ifndef OGL_RENDER_COMMAND_H
#define OGL_RENDER_COMMAND_H

#include <functional>

/// 渲染命令，封装一个渲染任务。
struct RenderCommand {
    std::function<void()> execute_func;
};

#endif //OGL_RENDER_COMMAND_H
