//
// Created by Nemo li on 2023/12/7.
//

#ifndef OGL_LUA_BINDING_H
#define OGL_LUA_BINDING_H

#include <sol/sol.hpp>

class LuaBinding {
public:
    /// 初始化
    /// \param package_path 设置lua文件搜索目录
    static void Init(std::string package_path);

    /// 绑定引擎所有类到Lua
    static void BindLua();

    /// 执行Lua脚本文件
    /// \param script_file_path
    static void RunLuaFile(const std::string &script_file_path);

    /// 调用Lua函数
    /// \param function_name
    /// \return
    static sol::protected_function_result CallLuaFunction();

    static sol::state &sol_state() { return sol_state_; };

private:
    static sol::state sol_state_;
};


#endif //OGL_LUA_BINDING_H
