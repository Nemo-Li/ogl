//
// Created by Nemo li on 2021/12/9.
//

#include <iostream>
#include "UI.h"

UI::UI(GLFWwindow *window, float width, float height) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    this->width = width;
    this->height = height;
    imCamera = ImCamera();
}

UI::UI() {

}

void UI::renderUI() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static ImVec4 v1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
    static ImVec4 v2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
    static ImVec4 v3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
    static ImVec4 v4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    modelMatrix = glm::mat4x4(
            v1.x, v2.x, v3.x, v4.x,
            v1.y, v2.y, v3.y, v4.y,
            v1.z, v2.z, v3.z, v4.z,
            v1.w, v2.w, v3.w, v4.w
    );

    static ImVec4 texv1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
    static ImVec4 texv2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
    static ImVec4 texv3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
    static ImVec4 texv4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    textureMatrix = glm::mat4x4(
            texv1.x, texv2.x, texv3.x, texv4.x,
            texv1.y, texv2.y, texv3.y, texv4.y,
            texv1.z, texv2.z, texv3.z, texv4.z,
            texv1.w, texv2.w, texv3.w, texv4.w
    );

    ImGui::SetNextWindowBgAlpha(0.15f);
    ImGui::Begin(
            "matrix demo");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
            "model matrix:");               // Display some text (you can use a format strings too)

    ImGui::DragFloat4("x", (float *) &v1, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("y", (float *) &v2, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("z", (float *) &v3, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("w", (float *) &v4, 0.01f, 0.0f, 1.0f);

    if (ImGui::Button("reset")) {
        v1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
        v2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
        v3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
        v4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (ImGui::DragFloat("theta", &theta, 0.01f, 0.0f, 3.1415926f)) {
        v1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
        v2 = ImVec4(0.0f, cos(theta), -sin(theta), 0.0f);
        v3 = ImVec4(0.0f, sin(theta), cos(theta), 0.0f);
        v4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    ImGui::Text(
            "texture matrix:");               // Display some text (you can use a format strings too)

    ImGui::DragFloat4("texturex", (float *) &texv1, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("texturey", (float *) &texv2, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("texturez", (float *) &texv3, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("texturew", (float *) &texv4, 0.01f, 0.0f, 1.0f);

    ImGui::ColorEdit3("3d window color", (float *) &left_window_color);

    if (ImGui::RadioButton("perspective", &projection_type, 0)) {
        perspectiveChange();
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", &projection_type, 1)) {
        orthoChange();
    }

    ImGui::Text("switch demo type");               // Display some text (you can use a format strings too)

    ImGui::RadioButton("mvp matrix", &demo_type, 0);
    ImGui::SameLine();
    ImGui::RadioButton("text demo", &demo_type, 1);

    ImGui::End();

    ImGui::SetNextWindowBgAlpha(0.15f);

    // 3. Show another simple window.
    if (projection_type == 1)
        ImGui::BeginDisabled();
    ImGui::Begin(
            "Projection Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Demonstration projection effect!");

    if (ImGui::DragFloat("projection_fov", &projection_fov, 1.0f, 1.0f, 90.0f)) {
        perspectiveChange();
    }
    if (ImGui::DragFloat("projection_near", &projection_near, 0.2f, 0.0f, 4.0f)) {
        perspectiveChange();
    }
    if (ImGui::DragFloat("projection_far", &projection_far, 0.2f, 0.0f, 4.0f)) {
        perspectiveChange();
    }
    ImGui::End();
    if (projection_type == 1)
        ImGui::EndDisabled();


    ImGui::SetNextWindowBgAlpha(0.15f);
    if (projection_type == 0)
        ImGui::BeginDisabled();
    ImGui::Begin("Orthogonal");

    ImGui::Text("Orthogonal projection effect!");

    if (ImGui::DragFloat("left", &ortho_left, 0.1f, -1.0f, 1.0f)) {
        orthoChange();
    }
    if (ImGui::DragFloat("right", &ortho_right, 0.1f, -1.0f, 1.0f)) {
        orthoChange();
    }
    if (ImGui::DragFloat("top", &ortho_top, 0.1f, -1.0f, 1.0f)) {
        orthoChange();
    }
    if (ImGui::DragFloat("bottom", &ortho_bottom, 0.1f, -1.0f, 1.0f)) {
        orthoChange();
    }
    if (ImGui::DragFloat("near", &ortho_near, 0.1, 0.0f, 4.0f)) {
        orthoChange();
    }
    if (ImGui::DragFloat("far", &ortho_far, 0.1f, 0.0f, 4.0f)) {
        orthoChange();
    }

    if (ImGui::Button("camera"))
        imCamera.WetCamera_window = true;

    ImGui::End();
    if (projection_type == 0)
        ImGui::EndDisabled();

    imCamera.draw();

    // Rendering
    ImGui::Render();
}

void UI::draw() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::addOnProjectionListener(OnProjectionListener *onProjectionListener) {
    listeners.push_back(onProjectionListener);
}

void UI::orthoChange() {
    for (auto onProjectionListener: listeners) {
        onProjectionListener->onOrthoChange(ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near, ortho_far);
    }
}

void UI::perspectiveChange() {
    for (auto onProjectionListener: listeners) {
        onProjectionListener->onPerspectiveChange(projection_fov, projection_near, projection_far, width,
                                                  height);
    }
}
