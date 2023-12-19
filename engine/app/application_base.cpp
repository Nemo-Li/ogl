//
// Created by Nemo li on 2023/12/13.
//

#include <lua_binding/lua_binding.h>
#include <control/input.h>
#include <component/game_object.h>
#include <renderer/camera.h>
#include <renderer/mesh_renderer.h>
#include <render_async/render_task_producer.h>
#include <render_async/render_task_consumer.h>
#include <renderer/terrain.h>
#include "application_base.h"
#include "helpers/time.h"
#include "helpers/debug.h"
#include "component/component.h"
#include "renderer/particle_system.h"

void ApplicationBase::Init() {
    Debug::Init();
    DEBUG_LOG_INFO("game start");

    InitLuaBinding();
    LoadConfig();

    Time::Init();

    //初始化图形库，例如glfw
    InitGraphicsLibraryFramework();

    UpdateScreenSize();
}

void ApplicationBase::InitLuaBinding() {
    //设置lua搜索目录
    LuaBinding::Init(
            ";../example/combine/?.lua;../engine_lua/?.lua;../engine_lua/helpers/?.lua;../engine_lua/app/?.lua;../engine_lua/component/?.lua;../engine_lua/renderer/?.lua");
    //绑定引擎所有类到Lua
    LuaBinding::BindLua();
    //执行lua
    LuaBinding::RunLuaFile("../example/combine/config.lua");
}

void ApplicationBase::LoadConfig() {
    sol::state &sol_state = LuaBinding::sol_state();
    title_ = sol_state["Config"]["title"];
    data_path_ = sol_state["Config"]["data_path"];
}

void ApplicationBase::Run() {
    LuaBinding::RunLuaFile("../example/combine/main.lua");
    //调用lua main()
    LuaBinding::CallLuaFunction();
}

void ApplicationBase::Update() {
    Time::Update();
    UpdateScreenSize();

    GameObject::Foreach([](GameObject *game_object) {
        if (game_object->active()) {
            game_object->ForeachComponent([](Component *component) {
                component->Update();
            });
        }
    });

    Input::Update();
}

void ApplicationBase::Render() {
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&]() {
        GameObject::Foreach([](GameObject *game_object) {
            if (!game_object->active()) {
                return;
            }

            auto *mesh_renderer = game_object->GetComponent<MeshRenderer>();
            if (mesh_renderer != nullptr) {
                mesh_renderer->Render();
            }

            auto *particleSystem = game_object->GetComponent<ParticleSystem>();
            if (particleSystem != nullptr) {
                particleSystem->Render();
            }

            auto *terrain = game_object->GetComponent<Terrain>();
            if (terrain != nullptr) {
                terrain->Render();
            }
        });
    });
}

void ApplicationBase::OneFrame() {
    Update();
    Render();

    //发出特殊任务：渲染结束
    RenderTaskProducer::ProduceRenderTaskEndFrame();
}

void ApplicationBase::UpdateScreenSize() {
    RenderTaskProducer::ProduceRenderTaskUpdateScreenSize();
}

void ApplicationBase::Exit() {
    RenderTaskConsumer::Exit();
}

void ApplicationBase::InitGraphicsLibraryFramework() {

}
