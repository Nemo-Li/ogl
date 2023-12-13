//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_APPLICATION_EDITOR_H
#define OGL_APPLICATION_EDITOR_H

#include "application_base.h"

struct GLFWwindow;

class ApplicationEditor : public ApplicationBase {
public:
    ApplicationEditor() : ApplicationBase() {}

    ~ApplicationEditor() override = default;

    void Run() override;

public:
    /// 初始化图形库，例如glfw
    void InitGraphicsLibraryFramework() override;

    void Exit() override;

private:
    //编辑器窗口
    GLFWwindow *editor_glfw_window_;
    //游戏窗口
    GLFWwindow *game_glfw_window_;
};


#endif //OGL_APPLICATION_EDITOR_H
