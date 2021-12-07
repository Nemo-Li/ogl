#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <stb_image.h>
#include "Shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

unsigned int VAO;
unsigned int texture1;
unsigned int texture2;


//顶点数组对象：Vertex Array Object，VAO
//顶点缓冲对象：Vertex Buffer Object，VBO
//索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
//--------------------------------------------------------------

//Using VAOs is required in the core profile. From the OpenGL 3.3 spec, page 342, in the section E.2.2 "Removed Features":
//The default vertex array object (the name zero) is also deprecated.
//This means that you can't set up vertex attributes without creating and binding your own VAO.

void initVAO();

void initTexture();

void renderShader(Shader ourShader);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void curse_pos_callback(GLFWwindow *window, double x, double y) {
//    ourShader.setVec2("mouse", glm::vec2(x, y));
//    std::cout << "(pos:" << x << "," << y << ")" << std::endl;
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 使用回调函数
    glfwSetCursorPosCallback(window, curse_pos_callback);

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

    initTexture();
    initVAO();
    Shader ourShader("../res/shader/shader.vert", "../res/shader/Heartfelt.frag");

    ourShader.use();
    ourShader.setVec2("resolution", glm::vec2(SCR_WIDTH * 2, SCR_HEIGHT * 2));
    // 使用着色器类设置
    ourShader.setInt("iChannel0", 0);

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

        glm::mat4x4 model(
                v1.x, v2.x, v3.x, v4.x,
                v1.y, v2.y, v3.y, v4.y,
                v1.z, v2.z, v3.z, v4.z,
                v1.w, v2.w, v3.w, v4.w
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

//        ourShader.use();
//        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
//        renderShader(ourShader);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ourShader.use();
        auto timeValue = (float) glfwGetTime();
        ourShader.setFloat("time", timeValue);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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

void renderShader(Shader ourShader) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ourShader.use();
    auto timeValue = (float) glfwGetTime();
    ourShader.setFloat("time", timeValue);
//    glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    // 纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
    unsigned char *data = stbi_load("../res/pic/rain.png", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
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