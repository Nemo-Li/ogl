//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_APPLICATION_EDITOR_H
#define OGL_APPLICATION_EDITOR_H

#include "application_base.h"
#include "data_struct/tree.h"

struct GLFWwindow;

class ApplicationEditor : public ApplicationBase {
public:
    ApplicationEditor() : ApplicationBase() {}

    ~ApplicationEditor() override = default;

    void Run() override;

    /// 绘制节点结构
    /// \param node
    /// \param label
    /// \param base_flags
    void DrawHierarchy(Tree::Node *node, const char *label, int base_flags);

public:
    /// 初始化图形库，例如glfw
    void InitGraphicsLibraryFramework() override;

    void Exit() override;

private:
    //编辑器窗口
    GLFWwindow *editor_glfw_window_;
    //游戏窗口
    GLFWwindow *game_glfw_window_;

    //记录Hierarchy当前选中的Node
    Tree::Node *selected_node_ = nullptr;
};


#endif //OGL_APPLICATION_EDITOR_H
