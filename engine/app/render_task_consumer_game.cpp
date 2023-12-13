//
// Created by Nemo li on 2023/12/13.
//

#include "render_task_consumer_game.h"

#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include "GLFW/glfw3.h"

RenderTaskConsumerGame::RenderTaskConsumerGame(GLFWwindow *window) : RenderTaskConsumerBase(), window_(window) {

}

RenderTaskConsumerGame::~RenderTaskConsumerGame() {}

void RenderTaskConsumerGame::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
}

void RenderTaskConsumerGame::GetFramebufferSize(int &width, int &height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerGame::SwapBuffer() {
    glfwSwapBuffers(window_);
}
