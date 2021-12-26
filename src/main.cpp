#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <stb_image.h>
#include "Shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <pthread.h>
#include "Grid.h"
#include "Frustum.h"
#include "UI.h"
#include "Texture.h"
#include "Rectangle.h"
#include "Orthogonal.h"
#include "CameraTexture.h"

//顶点数组对象：Vertex Array Object，VAO
//顶点缓冲对象：Vertex Buffer Object，VBO
//索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
//--------------------------------------------------------------

//Using VAOs is required in the core profile. From the OpenGL 3.3 spec, page 342, in the section E.2.2 "Removed Features":
//The default vertex array object (the name zero) is also deprecated.
//This means that you can't set up vertex attributes without creating and binding your own VAO.
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

// settings
int width = 3360;
int height = 2010;
bool l_button_down;

float projection_fov = 45;
float projection_near = 0.1f;
float projection_far = 4.0f;

double down_y_position;
double down_x_position;

glm::mat4 threeDModelMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
};

void rotateModel(double deltaX, double deltaY) {
    float angleX = deltaX / 1920 * 800;
    float angleY = deltaY / 1920 * 800;

    threeDModelMatrix = glm::rotate(threeDModelMatrix, glm::radians(angleX), glm::vec3(0.0f, 1.0f, 0.0f));
    threeDModelMatrix = glm::rotate(threeDModelMatrix, -glm::radians(angleY), glm::vec3(1.0f, 0.0f, 0.0f));
}

void curse_pos_callback(GLFWwindow *window, double x, double y) {
    if (l_button_down) {
//        cout << "down_x_position:" << down_x_position << " down_y_position" << down_y_position << endl;
        if (down_x_position < width / 4 || down_y_position > height / 4) {
            return;
        }
        // do your drag here
        double deltaX = x - down_x_position;
        double deltaY = y - down_y_position;
//        cout << "x移动距离:" << deltaX << " y移动距离：" << deltaY << endl;
        rotateModel(deltaX, deltaY);
        down_x_position = x;
        down_y_position = y;
    }
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action) {
            glfwGetCursorPos(window, &down_x_position, &down_y_position);
            l_button_down = true;
            cout << "GLFW_PRESS" << endl;
        } else if (GLFW_RELEASE == action) {
            l_button_down = false;
            cout << "GLFW_RELEASE" << endl;
        }
    }
}

float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // 右下
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 左下
        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // 左上
};

unsigned int indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};

/* Matrices */
glm::vec3 cam_position = glm::vec3(3.0f, 3.0f, 3.0f);
glm::vec3 cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 threed_view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);

glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), cam_look_at, cam_up);

//透视投影，观察纹理图片的视角，可变
glm::mat4 projection_matrix = glm::perspectiveFov(glm::radians(projection_fov), float(width), float(height),
                                                  projection_near,
                                                  projection_far);

//透视投影，固定的3维空间视角
glm::mat4 projection_matrix_threed = glm::perspectiveFov(glm::radians(45.0f), float(width), float(height), 0.1f,
                                                         10.0f);

//正交投影， 从结果来看是标准化的
glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
//    glfwGetFramebufferSize(window, &width, &height);
    cout << "窗口的大小为:" << "width " << width << "height " << height << endl;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 使用鼠标移动回调函数
    glfwSetCursorPosCallback(window, curse_pos_callback);
    // 设置鼠标点击回调
    glfwSetMouseButtonCallback(window, mouse_callback);
    // 鼠标滚动
    glfwSetScrollCallback(window, mouse_scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

//    pthread_t tid;
//    pthread_create(&tid, nullptr, say_hello, (void *) window);

    Texture texture;
    texture.load("res/pic/dragon.jpeg");

    UI ui(window, width, height);

    Shader threeDShader("../res/shader/shader3d.vert", "../res/shader/shader3d.frag");
    Shader ourShader("../res/shader/shader.vert", "../res/shader/shader.frag");
    Shader cameraShader("../res/shader/cameraShader.vert", "../res/shader/cameraShader.frag");

    Rectangle rectangle = Rectangle(&ourShader);
    rectangle.initVAO();
    rectangle.modelMatrix = &ui.modelMatrix;
    rectangle.viewMatrix = &view_matrix;
    rectangle.projectionMatrix = &projection_matrix;
    rectangle.textureMatrix = &ui.textureMatrix;
    rectangle.setTexture(&texture);
    ui.addOnProjectionListener(&rectangle);

    Grid grid(&threeDShader, &threeDModelMatrix);
    grid.initVAO();

    Rectangle rectangle3d(&threeDShader);
    rectangle3d.initVAO();
    rectangle3d.modelMatrix = &ui.modelMatrix;
    rectangle3d.modelTransMatrix = &threeDModelMatrix;
    rectangle3d.viewMatrix = &threed_view_matrix;
    rectangle3d.projectionMatrix = &projection_matrix_threed;
    rectangle3d.useTexture = true;
    rectangle3d.setTexture(&texture);

    Frustum frustum(&threeDShader, &threeDModelMatrix);
    frustum.initFrustum(projection_fov, float(width) / float(height), projection_near, projection_far);
    ui.addOnProjectionListener(&frustum);

    Orthogonal orthogonal(&threeDShader, &threeDModelMatrix);
    orthogonal.initVAO(-1.0f, 1.0f, 1.0f, -1.0f, 0.1f, 4.0f);
    ui.addOnProjectionListener(&orthogonal);

    CameraTexture cameraTexture;
    cameraTexture.initVAO(&cameraShader);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        ui.renderUI();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable scissor test
        glEnable(GL_SCISSOR_TEST);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Upper left view (TOP VIEW)
        glViewport(0, height / 2, width / 2, height / 2);
        glScissor(0, height / 2, width / 2, height / 2);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rectangle.draw();

        //upper right view
        glViewport(width / 2, height / 2, width / 2, height / 2);
        glScissor(width / 2, height / 2, width / 2, height / 2);
        glClearColor(ui.left_window_color.x, ui.left_window_color.y, ui.left_window_color.z, ui.left_window_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rectangle3d.draw();
        grid.draw();
        if (ui.projection_type == 0) {
            frustum.draw();
        } else {
            orthogonal.draw();
        }

        glViewport(width / 2, 0, width / 2, height / 2);
        glScissor(width / 2, 0, width / 2, height / 2);
        glClearColor(0.887, 0.925, 0.801, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cameraTexture.draw();

        glViewport(0, 0, width / 2, height / 2);
        glScissor(0, 0, width / 2, height / 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ui.draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    double cursorX;
    double cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    if (cursorX < width / 4 || cursorY > height / 4) {
        return;
    }

    float scale = 1.0f;
    scale -= y_offset / 10.0f;
    threeDModelMatrix = glm::scale(threeDModelMatrix, glm::vec3(scale, scale, scale));
}