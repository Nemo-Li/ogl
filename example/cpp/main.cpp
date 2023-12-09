#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <control/key_code.h>
#include <helpers/screen.h>
#include "framework/application.h"
#include "renderer/mesh_filter.h"
#include "renderer/shader.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/camera.h"

#include "component/component.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/input.h"
#include "login_scene.h"

using namespace std;

int main() {
    Application::set_title("main");
    Application::set_data_path("../res/");
    Application::Init();

    GameObject *go_login_scene = new GameObject("LoginSceneGo");
    go_login_scene->AddComponent<LoginScene>();

    Application::Run();
}