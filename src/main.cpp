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

GLFWwindow *window;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

GLuint m_boneLocation[MAX_BONES];
vector<Matrix4f> Transforms;

Model *pModel = nullptr;
Shader *shader = nullptr;
long long m_startTime;

/* Matrices */
glm::vec3 cam_position = glm::vec3(0.0f, 1.0f, 1.2f);
glm::vec3 cam_look_at = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 world_matrix = glm::mat4(1.0f);
glm::mat4 view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);
glm::mat4 projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(WINDOW_WIDTH), float(WINDOW_HEIGHT), 0.1f,
                                                  10.0f);

void window_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(width), float(height), 0.1f, 10.0f);

    if (shader != nullptr) {
        shader->setUniformMatrix4fv("viewProj", projection_matrix * view_matrix);
    }
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
    pModel = new Model("res/models/TSZ-Attack.FBX");

    /* Create and apply basic shader */
    shader = new Shader("Basic.vert", "Basic.frag");
    shader->apply();

    world_matrix = glm::scale(world_matrix, glm::vec3(.5, .5, .5));
    world_matrix = glm::rotate(world_matrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    world_matrix = glm::rotate(world_matrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    shader->setUniformMatrix4fv("world", world_matrix);
    shader->setUniformMatrix3fv("normalMatrix", glm::inverse(glm::transpose(glm::mat3(world_matrix))));
    shader->setUniformMatrix4fv("viewProj", projection_matrix * view_matrix);

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

    shader->setUniform1i("tex_sampler", 0);
    pModel->Draw();
}

void update() {

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

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