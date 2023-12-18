//
// Created by Nemo li on 2023/12/16.
//

#include <glm/ext.hpp>
#include "control.h"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3(0, -1.5, 5);

float initialFoV = 45.0f;

void computeMatricesFromInputs() {
    // Up vector
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0); //glm::cross(right, direction);


    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 1028.0f / 1080.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix = glm::lookAt(
            position,           // Camera is here
            glm::vec3(0, 0, 0), // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
    );
}