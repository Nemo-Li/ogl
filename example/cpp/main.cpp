#include <GLFW/glfw3.h>
#include "framework/application.h"
#include "renderer/camera.h"

#include "component/game_object.h"
#include "component/transform.h"
#include "login_scene.h"
#include "mesh_animation_scene.h"

using namespace std;

int main() {
    Application::set_title("main");
    Application::set_data_path("../res/");
    Application::Init();

    GameObject *go_login_scene = new GameObject("LoginSceneGo");
    go_login_scene->AddComponent<MeshAnimationScene>();

    Application::Run();
}