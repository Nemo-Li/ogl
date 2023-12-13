//
// Created by Nemo li on 2023/12/12.
//

#ifndef OGL_RENDER_TASK_QUEUE_H
#define OGL_RENDER_TASK_QUEUE_H

#include <spscqueue/include/rigtorp/SPSCQueue.h>

class RenderTaskBase;

class RenderTaskQueue {
public:
    /// 添加任务到队列
    /// \param render_task
    static void Push(RenderTaskBase *render_task) {
        render_task_queue_.push(render_task);
    }

    /// 队列中是否没有了任务
    /// \return
    static bool Empty() {
        return render_task_queue_.empty();
    }

    /// 获取队列中第一个任务
    /// \return
    static RenderTaskBase *Front() {
        return *(render_task_queue_.front());
    }

    /// 弹出队列中第一个任务
    static void Pop() {
        render_task_queue_.pop();
    }

    /// 获取队列中的任务数量
    static size_t Size() {
        return render_task_queue_.size();
    }

private:
    //渲染任务队列
    static rigtorp::SPSCQueue<RenderTaskBase *> render_task_queue_;
};


#endif //OGL_RENDER_TASK_QUEUE_H
