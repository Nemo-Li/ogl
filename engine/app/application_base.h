//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_APPLICATION_BASE_H
#define OGL_APPLICATION_BASE_H

#include <string>
#include <utility>

class ApplicationBase {
public:
    ApplicationBase() = default;

    virtual ~ApplicationBase() = default;

    void set_title(std::string title) { title_ = std::move(title); }

    const std::string &data_path() { return data_path_; }

    void set_data_path(std::string data_path) { data_path_ = std::move(data_path); }

    /// 初始化OpenGL
    virtual void Init();

    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework();

    virtual void InitLuaBinding();

    virtual void LoadConfig();

    virtual void Run();

    /// 一帧
    virtual void OneFrame();

    virtual void UpdateScreenSize();

    /// 每一帧内逻辑代码。
    virtual void Update();

    /// 逻辑代码执行后，应用到渲染。
    virtual void Render();

    virtual void Exit();

protected:
    std::string title_;//标题栏显示

    std::string data_path_;//资源目录
};


#endif //OGL_APPLICATION_BASE_H
