//
// Created by Nemo li on 2023/12/13.
//

#include "render_task_consumer_editor.h"

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <helpers/debug.h>

RenderTaskConsumerEditor::RenderTaskConsumerEditor(GLFWwindow *window) : RenderTaskConsumerBase(), window_(window) {

}

RenderTaskConsumerEditor::~RenderTaskConsumerEditor() {}

void RenderTaskConsumerEditor::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    //创建全局FBO，将整个游戏渲染到FBO，提供给编辑器，作为Game视图显示
    GLuint frame_buffer_object_id = 0;
    glGenFramebuffers(1, &frame_buffer_object_id);
    __CHECK_GL_ERROR__
    if (frame_buffer_object_id == 0) {
        DEBUG_LOG_ERROR("CreateFBO FBO Error!");
        return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_object_id);
    __CHECK_GL_ERROR__

    //创建颜色纹理 Attach到FBO颜色附着点上
    glGenTextures(1, &color_texture_id_);
    glBindTexture(GL_TEXTURE_2D, color_texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1028, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    __CHECK_GL_ERROR__
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id_, 0);
    __CHECK_GL_ERROR__

    //创建深度纹理 Attach到FBO深度附着点上
    glGenTextures(1, &depth_texture_id_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1028, 1080, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    __CHECK_GL_ERROR__
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id_, 0);
    __CHECK_GL_ERROR__

    //检测帧缓冲区完整性
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    __CHECK_GL_ERROR__
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !",
                        status);//36055 = 0x8CD7 GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 附着点没有东西
        return;
    }
}

void RenderTaskConsumerEditor::GetFramebufferSize(int &width, int &height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerEditor::SwapBuffer() {
    glfwSwapBuffers(window_);
}
