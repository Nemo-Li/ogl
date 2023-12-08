//
// Created by Nemo li on 2023/12/7.
//

#include "lua_binding/lua_binding.h"

int main() {
    //设置lua搜索目录
    LuaBinding::Init(
            ";../example/?.lua;../engine_lua/?.lua;../engine_lua/helpers/?.lua;../engine_lua/framework/?.lua;../engine_lua/component/?.lua");
    //绑定引擎所有类到Lua
    LuaBinding::BindLua();
    //执行lua
    LuaBinding::RunLuaFile("../example/nemo.lua");
    //调用lua main()
    LuaBinding::CallLuaFunction();

    return 0;
}
