#include <GLFW/glfw3.h>
#include "app/application_editor.h"
#include "app/application.h"

using namespace std;

int main() {
    Application::Init(new ApplicationEditor());
    Application::Run();
    return 0;
}