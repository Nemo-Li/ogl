//
// Created by Nemo li on 2023/12/13.
//

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <render_async/render_task_consumer.h>
#include "application_game.h"
#include "control/input.h"
#include "helpers/debug.h"
#include "render_task_consumer_game.h"

static void error_callback(int error, const char *description) {
    DEBUG_LOG_ERROR("glfw error:{} description:{}", error, description);
}

/// 键盘回调
/// \param window
/// \param key
/// \param scancode
/// \param action
/// \param mods
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Input::RecordKey(key, action);
}

/// 鼠标按键回调
/// \param window
/// \param button
/// \param action
/// \param mods
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Input::RecordKey(button, action);
}

/// 鼠标移动回调
/// \param window
/// \param x
/// \param y
static void mouse_move_callback(GLFWwindow *window, double x, double y) {
    Input::set_mousePosition(x, y);
}

/// 鼠标滚轮回调
/// \param window
/// \param x
/// \param y
static void mouse_scroll_callback(GLFWwindow *window, double x, double y) {
    Input::RecordScroll(y);
}

void ApplicationGame::InitGraphicsLibraryFramework() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        DEBUG_LOG_ERROR("glfw init failed!");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfw_window_ = glfwCreateWindow(960, 640, title_.c_str(), nullptr, nullptr);
    if (!glfw_window_) {
        DEBUG_LOG_ERROR("glfwCreateWindow error!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(glfw_window_, key_callback);
    glfwSetMouseButtonCallback(glfw_window_, mouse_button_callback);
    glfwSetScrollCallback(glfw_window_, mouse_scroll_callback);
    glfwSetCursorPosCallback(glfw_window_, mouse_move_callback);

    //初始化渲染任务消费者(单独渲染线程)
    RenderTaskConsumer::Init(new RenderTaskConsumerGame(glfw_window_));
}

void ApplicationGame::Run() {
    ApplicationBase::Run();

    while (true) {
        if (glfwWindowShouldClose(glfw_window_)) {
            break;
        }
        OneFrame();
        glfwPollEvents();
    }
    Exit();
}

void ApplicationGame::Exit() {
    ApplicationBase::Exit();
    glfwDestroyWindow(glfw_window_);
    glfwTerminate();
}