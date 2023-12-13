//
// Created by Nemo li on 2023/12/6.
//

#include "application.h"
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <renderer/camera.h>
#include <control/input.h>
#include <render_async/render_task_consumer.h>
#include <render_async/render_task_producer.h>
#include "component/game_object.h"
#include "renderer/mesh_renderer.h"
#include "helpers/debug.h"
#include "helpers/time.h"

std::string Application::data_path_;
std::string Application::title_;
GLFWwindow *Application::glfw_window_;

/// 键盘回调
/// \param window
/// \param key
/// \param scancode
/// \param action
/// \param mods
static void key_callback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
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

void Application::Init() {
    Debug::Init();
    DEBUG_LOG_INFO("game start");
    Time::Init();
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
    RenderTaskConsumer::Init(glfw_window_);

    UpdateScreenSize();
}

void Application::Run() {
    while (true) {
        if (glfwWindowShouldClose(glfw_window_)) {
            break;
        }

        Update();
        Render();

        //发出特殊任务：渲染结束
        RenderTaskProducer::ProduceRenderTaskEndFrame();

        glfwPollEvents();
    }

    RenderTaskConsumer::Exit();

    glfwDestroyWindow(glfw_window_);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Application::Update() {
    Time::Update();
    UpdateScreenSize();

    GameObject::Foreach([](GameObject *game_object) {
        if (game_object->active()) {
            game_object->ForeachComponent([](Component *component) {
                component->Update();
            });
        }
    });

    Input::Update();
}

void Application::Render() {
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&]() {
        GameObject::Foreach([](GameObject *game_object) {
            if (!game_object->active()) {
                return;
            }
            MeshRenderer *mesh_renderer = game_object->GetComponent<MeshRenderer>();
            if (mesh_renderer == nullptr) {
                return;
            }
            mesh_renderer->Render();
        });
    });
}

void Application::UpdateScreenSize() {
    RenderTaskProducer::ProduceRenderTaskUpdateScreenSize();
}