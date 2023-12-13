//
// Created by Nemo li on 2023/12/12.
//

#include "render_task_consumer.h"
#include "render_task_consumer_base.h"

RenderTaskConsumerBase *RenderTaskConsumer::instance_ = nullptr;


void RenderTaskConsumer::Init(RenderTaskConsumerBase *instance) {
    instance_ = instance;
    instance_->Init();
}

RenderTaskConsumerBase *RenderTaskConsumer::Instance() {
    return instance_;
}

void RenderTaskConsumer::Exit() {
    instance_->Exit();
}