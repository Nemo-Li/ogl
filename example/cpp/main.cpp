#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "framework/application.h"
#include "renderer/mesh_filter.h"
#include "renderer/shader.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/camera.h"

#include "component/component.h"
#include "component/game_object.h"
#include "component/transform.h"

using namespace std;

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *window;

//初始化OpenGL
void init_opengl() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(960, 640, "Simple example", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
}

int main(void) {
    Application::set_data_path("../res/");
    init_opengl();

    //创建主相机
    GameObject *go_camera = new GameObject("main_camera");
    auto go_transform = go_camera->AddComponent<Transform>();
    go_transform->set_position(glm::vec3(0, 0, 10));
    auto camera = go_camera->AddComponent<Camera>();

    //创建GameObject
    GameObject *go = new GameObject("fish_soup_pot");

    //挂上 Transform 组件
    auto transform = go->AddComponent<Transform>();
    transform->set_position(glm::vec3(0, -1, 0));

    //挂上 MeshFilter 组件
    auto mesh_filter = dynamic_cast<MeshFilter *>(go->AddComponent<MeshFilter>());
    mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

    //挂上MeshRenderer 组件
    auto mesh_renderer = dynamic_cast<MeshRenderer *>(go->AddComponent<MeshRenderer>());

    //设置材质
    Material *material = new Material();
    material->Load("material/fishsoup_pot.mat");
    mesh_renderer->SetMaterial(material);

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        //旋转物体
        static float rotate_eulerAngle = 0.f;
        rotate_eulerAngle += 0.1f;
        glm::vec3 rotation = transform->rotation();
        rotation.y = rotate_eulerAngle;
        transform->set_rotation(rotation);

        camera->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        camera->SetPerspective(60, ratio, 1, 1000);

//        mesh_renderer->Render();

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}