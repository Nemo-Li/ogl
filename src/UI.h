//
// Created by Nemo li on 2021/12/9.
//

#ifndef OGL_UI_H
#define OGL_UI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OnProjectionListener.h"
#include <vector>

using namespace std;

class UI {
public:
    // Our state
    float theta = 0;
    ImVec4 left_window_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float projection_fov = 45;
    float projection_near = 0.1f;
    float projection_far = 4.0f;
    float width;
    float height;
    int projection_type = 0;

    glm::mat4x4 modelMatrix;
    glm::mat4x4 textureMatrix;

    std::vector<OnProjectionListener *> listeners;

    void addOnProjectionListener(OnProjectionListener *onProjectionListener);

    UI();

    UI(GLFWwindow *window, float width, float height);

    void renderUI();

    void draw();
};


#endif //OGL_UI_H
