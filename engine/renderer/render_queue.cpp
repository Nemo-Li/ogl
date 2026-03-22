#include "render_queue.h"

std::vector<RenderCommand> RenderQueue::render_commands_;

void RenderQueue::Push(RenderCommand command) {
    render_commands_.push_back(command);
}

void RenderQueue::Execute() {
    for (auto &command : render_commands_) {
        if (command.execute_func) {
            command.execute_func();
        }
    }
}

void RenderQueue::Clear() {
    render_commands_.clear();
}
