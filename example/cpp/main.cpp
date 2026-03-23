#include <GLFW/glfw3.h>
#include "framework/application.h"
#include "renderer/camera.h"

#include "component/game_object.h"
#include "component/transform.h"
#include "login_scene.h"
#include "mesh_animation_scene.h"
#include "physics_scene.h"

using namespace std;

int main() {
    Application::set_title("main");
    Application::set_data_path("../res/");
    Application::Init();

    GameObject *go_physics_scene = new GameObject("PhysicsSceneGo");
    go_physics_scene->AddComponent<PhysicsScene>();

    Application::Run();
}