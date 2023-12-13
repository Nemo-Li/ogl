#include <GLFW/glfw3.h>
#include <app/application_game.h>
#include "app/application.h"

using namespace std;

int main() {
    Application::Init(new ApplicationGame());
    Application::Run();
    return 0;
}