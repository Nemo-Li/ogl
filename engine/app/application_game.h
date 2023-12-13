//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_APPLICATION_GAME_H
#define OGL_APPLICATION_GAME_H

#include "application_base.h"

struct GLFWwindow;

class ApplicationGame : public ApplicationBase {
public:
    ApplicationGame() : ApplicationBase() {}

    ~ApplicationGame() {}

    void Run();

public:
    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework() override;

    virtual void Exit() override;

private:
    GLFWwindow *glfw_window_;
};


#endif //OGL_APPLICATION_GAME_H
