/** 
 * Copyright (C) 2018 Tomasz Ga³aj
 **/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <helpers/util.h>
#include <zconf.h>

#include "rendering/Shader.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"
#include "camera/Camera.h"

GLFWwindow *window;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLuint m_boneLocation[MAX_BONES];
vector<Matrix4f> Transforms;

Model *pModel = nullptr;
Shader *shader = nullptr;
long long m_startTime;

/* Matrices */
glm::vec3 cam_position = glm::vec3(0.0f, 3.0f, 3.2f);
glm::vec3 cam_look_at = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 world_matrix = glm::mat4(1.0f);
//glm::mat4 view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);
glm::mat4 projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(WINDOW_WIDTH), float(WINDOW_HEIGHT), 0.1f,
                                                  10.0f);

bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void window_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(width), float(height), 0.1f, 10.0f);

    if (shader != nullptr) {
        shader->setUniformMatrix4fv("viewProj", projection_matrix * camera.GetViewMatrix());
    }
}

void mouse_scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera.ProcessMouseScroll(y_offset);
}

void curse_pos_callback(GLFWwindow *window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

    lastX = x;
    lastY = y;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        cout << "按键w按下" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int init() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Modern GL!", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    m_startTime = GetCurrentTimeMillis();
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, window_size_callback);
    // 使用鼠标移动回调函数
    glfwSetCursorPosCallback(window, curse_pos_callback);
    // 鼠标滚动
    glfwSetScrollCallback(window, mouse_scroll_callback);

    /* Initialize glad */
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Set the viewport */
    glClearColor(0.6784f, 0.8f, 1.0f, 1.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    return true;
}

int loadContent() {
    pModel = new Model("res/models/mill-scene.fbx");

    /* Create and apply basic shader */
    shader = new Shader("Basic.vert", "Basic.frag");
    shader->apply();

    world_matrix = glm::scale(world_matrix, glm::vec3(.26, .26, .26));
    world_matrix = glm::translate(world_matrix, glm::vec3(0, -5, 0));
//    world_matrix = glm::rotate(world_matrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    world_matrix = glm::rotate(world_matrix, glm::radians(-45.0f), glm::vec3(1, 0, 0));

    shader->setUniformMatrix4fv("world", world_matrix);
    shader->setUniformMatrix3fv("normalMatrix", glm::inverse(glm::transpose(glm::mat3(world_matrix))));
    shader->setUniformMatrix4fv("viewProj", projection_matrix * camera.GetViewMatrix());

    shader->setUniform3fv("cam_pos", cam_position);

    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
        m_boneLocation[i] = glGetUniformLocation(shader->getProgram(), Name);
    }

    Transforms.resize(pModel->getBoneNum());
    for (int i = 0; i < pModel->getBoneNum(); i++) {
        Matrix4f f = Matrix4f();
        f.InitIdentity();
        Transforms[i] = f;
    }

    for (uint i = 0; i < Transforms.size(); i++) {
        glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat *) Transforms[i]);
    }

    shader->setUniform1b("animation", pModel->hasAnimation);
    assert(glGetError() == GL_NO_ERROR);

    return true;
}

float GetRunningTime() {
    float RunningTime = (float) ((double) GetCurrentTimeMillis() - (double) m_startTime) / 1000.0f;
    return RunningTime;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->apply();
    float runningTime = GetRunningTime();
    pModel->BoneTransform(runningTime, Transforms);

    for (uint i = 0; i < Transforms.size(); i++) {
        glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat *) Transforms[i]);
    }

    shader->setUniformMatrix4fv("viewProj", projection_matrix * camera.GetViewMatrix());
    shader->setUniform1i("tex_sampler", 0);
    shader->setUniform1b("useColor", true);
    pModel->Draw(shader);
}

void update() {

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        /* Render here */
        render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int main() {
    if (!init())
        return -1;

    if (!loadContent())
        return -1;

    update();

    glfwTerminate();

    delete pModel;
    delete shader;

    return 0;
}