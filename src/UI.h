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

class UI {
public:
    // Our state
    bool show_another_window = false;
    float theta = 0;
    ImVec4 left_window_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glm::mat4x4 modelMatrix;
    glm::mat4x4 textureMatrix;

    UI();

    UI(GLFWwindow *window);

    void renderUI();

    void draw();
};


#endif //OGL_UI_H
