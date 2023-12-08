//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_COMPONENT_H
#define OGL_COMPONENT_H

#include <rttr/registration>
#include <sol/sol.hpp>
#include "helpers/debug.h"

class GameObject;

class Component {
public:
    Component();

    virtual ~Component();

    GameObject *game_object() {
        return game_object_;
    };

    void set_game_object(GameObject *game_object) {
        game_object_ = game_object;
    };

    /// 设置对应的Lua组件
    /// \param lua_component_instance
    void set_lua_component_instance(sol::table lua_component_instance) {
        lua_component_instance_ = lua_component_instance;
    }


    virtual void OnEnable();

    virtual void Awake();

    virtual void Update();

    /// 渲染之前
    virtual void OnPreRender();

    /// 渲染之后
    virtual void OnPostRender();

    virtual void OnDisable();

private:
    GameObject *game_object_;
    sol::table lua_component_instance_;

    /// 同步调用Lua组件函数
    /// \param function_name
    void SyncLuaComponent(const char *function_name);

RTTR_ENABLE();
};


#endif //OGL_COMPONENT_H
