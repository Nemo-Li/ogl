#ifndef OGL_RENDER_QUEUE_H
#define OGL_RENDER_QUEUE_H

#include <vector>
#include "render_command.h"

/// 渲染队列，用于收集渲染命令并统一执行。
class RenderQueue {
public:
    /// 提交渲染命令
    /// \param command
    static void Push(RenderCommand command);

    /// 执行队列中所有的渲染命令
    static void Execute();

    /// 清空队列
    static void Clear();

private:
    static std::vector<RenderCommand> render_commands_;
};

#endif //OGL_RENDER_QUEUE_H
