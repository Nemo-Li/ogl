//
// Created by Nemo li on 2023/12/12.
//

#include "render_task_queue.h"

//渲染任务队列
rigtorp::SPSCQueue<RenderTaskBase *> RenderTaskQueue::render_task_queue_(1024);
