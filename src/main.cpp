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
#include <array>

unsigned int VAO;
unsigned int threeDVAO;
unsigned int lineVAO;
unsigned int frustumVAO;

unsigned int texture1;
unsigned int texture2;
bool l_button_down;

glm::mat4 threeDModelMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
};

float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

//顶点数组对象：Vertex Array Object，VAO
//顶点缓冲对象：Vertex Buffer Object，VBO
//索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
//--------------------------------------------------------------

//Using VAOs is required in the core profile. From the OpenGL 3.3 spec, page 342, in the section E.2.2 "Removed Features":
//The default vertex array object (the name zero) is also deprecated.
//This means that you can't set up vertex attributes without creating and binding your own VAO.

void initVAO();

void init3DVAO();

void initLineVAO();

void initFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane);

void initTexture();

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// settings
int width = 3360;
int height = 2010;

void rotateModel(double deltaX, double deltaY) {
    float angleX = deltaX / 1920 * 10;
    float angleY = deltaY / 1920 * 10;

    threeDModelMatrix = glm::rotate(threeDModelMatrix, glm::radians(angleX), glm::vec3(0.0f, 1.0f, 0.0f));
    threeDModelMatrix = glm::rotate(threeDModelMatrix, glm::radians(angleY), glm::vec3(1.0f, 0.0f, 0.0f));
}

double down_y_position;
double down_x_position;

void curse_pos_callback(GLFWwindow *window, double x, double y) {
    if (l_button_down) {
        cout << "down_x_position:" << down_x_position << " down_y_position" << down_y_position << endl;
        if (down_x_position < width / 4 || down_y_position > height / 4) {
            return;
        }
        // do your drag here
        double deltaX = x - down_x_position;
        double deltaY = y - down_y_position;
        cout << "x移动距离:" << deltaX << " y移动距离：" << deltaY << endl;
        rotateModel(deltaX, deltaY);
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

float threeDVertices[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
};

unsigned int indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};

/* Matrices */
glm::vec3 cam_position = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);
glm::mat4 projection_matrix = glm::perspectiveFov(glm::radians(45.0f), float(width), float(height), 0.1f,
                                                  10.0f);

//正交投影， 从结果来看是标准化的
glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);

// 线程的运行函数
void *say_hello(void *window) {
    return nullptr;
}

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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

    // Our state
    bool show_another_window = false;
    float theta = 0;
    ImVec4 left_window_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    pthread_t tid;
    pthread_create(&tid, nullptr, say_hello, (void *) window);

    initTexture();
    initVAO();
    init3DVAO();
    initLineVAO();
    initFrustum(45.0f, 1, 0.1, 1);

    Shader threeDShader("../res/shader/shader3d.vert", "../res/shader/shader3d.frag");
    Shader ourShader("../res/shader/shader.vert", "../res/shader/shader.frag");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static ImVec4 v1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
        static ImVec4 v2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
        static ImVec4 v3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
        static ImVec4 v4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4x4 modelMatrix(
                v1.x, v2.x, v3.x, v4.x,
                v1.y, v2.y, v3.y, v4.y,
                v1.z, v2.z, v3.z, v4.z,
                v1.w, v2.w, v3.w, v4.w
        );

        static ImVec4 texv1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
        static ImVec4 texv2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
        static ImVec4 texv3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
        static ImVec4 texv4 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4x4 textureMatrix(
                texv1.x, texv2.x, texv3.x, texv4.x,
                texv1.y, texv2.y, texv3.y, texv4.y,
                texv1.z, texv2.z, texv3.z, texv4.z,
                texv1.w, texv2.w, texv3.w, texv4.w
        );

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

        ImGui::End();

        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin("Another Window",
                         &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable scissor test
        glEnable(GL_SCISSOR_TEST);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Upper left view (TOP VIEW)
        glViewport(0, height / 2, width / 2, height / 2);
        glScissor(0, height / 2, width / 2, height / 2);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        auto timeValue = (float) glfwGetTime();
        ourShader.setFloat("time", timeValue);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "viewMatrix"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projectionMatrix"), 1, GL_FALSE,
                           &ortho_matrix[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "textureMatrix"), 1, GL_FALSE, &textureMatrix[0][0]);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //upper right view
        glViewport(width / 2, height / 2, width / 2, height / 2);
        glScissor(width / 2, height / 2, width / 2, height / 2);
        glClearColor(left_window_color.x, left_window_color.y, left_window_color.z, left_window_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        threeDShader.use();
        threeDShader.setVec3("drawColor", 0.0f, 1.0f, 1.0f);

        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "modelMatrix"), 1, GL_FALSE, &threeDModelMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "viewMatrix"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "projectionMatrix"), 1, GL_FALSE,
                           &projection_matrix[0][0]);

        glBindVertexArray(threeDVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        threeDShader.setVec3("drawColor", 1.0f, 1.0f, 1.0f);
        glBindVertexArray(lineVAO);

        for (int i = 0; i < 80; ++i) {
//            glm::mat4 lineModel = glm::mat4(1.0f);
            glm::mat4 lineModel = threeDModelMatrix;
            if (i < 20) {
                lineModel = glm::translate(lineModel, glm::vec3(float(i) * 0.1f, 0.0, 0.0f));
            } else if (i < 40) {
                lineModel = glm::translate(lineModel, glm::vec3(float(20 - i) * 0.1f, 0.0, 0.0f));
            } else if (i < 60) {
                lineModel = glm::translate(lineModel, glm::vec3(0.0f, 0.0f, float(i - 40) * 0.1f));
                lineModel = glm::rotate(lineModel, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0f));
            } else {
                lineModel = glm::translate(lineModel, glm::vec3(0.0f, 0.0f, float(60 - i) * 0.1f));
                lineModel = glm::rotate(lineModel, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0f));
            }
            glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "modelMatrix"), 1, GL_FALSE, &lineModel[0][0]);
            glDrawArrays(GL_LINES, 0, 2);
        }

        glm::mat4 identity = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "modelMatrix"), 1, GL_FALSE, &threeDModelMatrix[0][0]);
//        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "viewMatrix"), 1, GL_FALSE, &identity[0][0]);
//        glUniformMatrix4fv(glGetUniformLocation(threeDShader.ID, "projectionMatrix"), 1, GL_FALSE,
//                           &identity[0][0]);

        threeDShader.setVec3("drawColor", 1.0f, 0.0f, 0.0f);
        glBindVertexArray(frustumVAO);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        threeDShader.setVec3("drawColor", 0.0f, 1.0f, 0.0f);
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void *) (sizeof(int) * 12));

        glViewport(0, 0, width, height / 2);
        glScissor(0, 0, width, height / 2);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

void initVAO() {
    unsigned int VBO;
    unsigned int EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(0);
    // 颜色属性
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
//    glEnableVertexAttribArray(1);
    // 纹理属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void init3DVAO() {
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &threeDVAO);
    glBindVertexArray(threeDVAO);

    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

float lineVertices[] = {
        0.0f, 0.0f, -2.0f,
        0.0f, 0.0f, 2.0f
};

void initLineVAO() {
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);

    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void initTexture() {
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../res/pic/dragon.jpeg", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
}

// constants
const float DEG2RAD = 3.1415926f / 180;
const float FOV_Y = 45.0f;              // vertical FOV in degree
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 100.0f;
const float CAMERA_ANGLE_X = 45.0f;     // pitch in degree
const float CAMERA_ANGLE_Y = -45.0f;    // heading in degree
const float CAMERA_DISTANCE = 25.0f;    // camera distance

void initFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane) {
    float tangent = tanf(fovY / 2 * DEG2RAD);
    float nearHeight = nearPlane * tangent;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = farPlane * tangent;
    float farWidth = farHeight * aspectRatio;

    // compute 8 vertices of the frustum
    float vertices[8][3];
    // near top right
    vertices[0][0] = nearWidth;
    vertices[0][1] = nearHeight;
    vertices[0][2] = -nearPlane;
    // near top left
    vertices[1][0] = -nearWidth;
    vertices[1][1] = nearHeight;
    vertices[1][2] = -nearPlane;
    // near bottom left
    vertices[2][0] = -nearWidth;
    vertices[2][1] = -nearHeight;
    vertices[2][2] = -nearPlane;
    // near bottom right
    vertices[3][0] = nearWidth;
    vertices[3][1] = -nearHeight;
    vertices[3][2] = -nearPlane;
    // far top right
    vertices[4][0] = farWidth;
    vertices[4][1] = farHeight;
    vertices[4][2] = -farPlane;
    // far top left
    vertices[5][0] = -farWidth;
    vertices[5][1] = farHeight;
    vertices[5][2] = -farPlane;
    // far bottom left
    vertices[6][0] = -farWidth;
    vertices[6][1] = -farHeight;
    vertices[6][2] = -farPlane;
    // far bottom right
    vertices[7][0] = farWidth;
    vertices[7][1] = -farHeight;
    vertices[7][2] = -farPlane;

    float colorLine1[4] = {0.7f, 0.7f, 0.7f, 0.7f};
    float colorLine2[4] = {0.2f, 0.2f, 0.2f, 0.7f};
    float colorPlane[4] = {0.5f, 0.5f, 0.5f, 0.5f};

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << "ixj " << i << "x" << j << " " << vertices[i][j] << endl;
        }
    }

    int indices[] = {
            0, 1, 3,
            1, 3, 2,
            5, 4, 7,
            5, 7, 6,
            0, 4,
            1, 5,
            2, 6,
            3, 7
    };

    unsigned int VBO;
    unsigned int EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &frustumVAO);
    glBindVertexArray(frustumVAO);
    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    // 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
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