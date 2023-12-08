//
// Created by Nemo li on 2023/12/7.
//

#include "lua_binding.h"
#include <glm/ext.hpp>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/scalar_multiplication.hpp>
#include <renderer/camera.h>
#include <helpers/screen.h>
#include "component/game_object.h"
#include "component/transform.h"
#include "renderer/material.h"
#include "renderer/mesh_filter.h"
#include "renderer/mesh_renderer.h"
#include "renderer/shader.h"
#include "renderer/texture2d.h"
#include "framework/application.h"

sol::state LuaBinding::sol_state_;

void LuaBinding::Init(std::string package_path) {
    sol_state_.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os,
                              sol::lib::math,
                              sol::lib::table, sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::utf8);
    //设置lua搜索目录
    sol::table package_table = sol_state_["package"];
    std::string path = package_table["path"];
    path.append(package_path);
    package_table["path"] = path;
}

namespace sol2 {
    /// 将lua中的table转换为std::vector
    /// \tparam elementType
    /// \param t
    /// \return
    template<typename elementType>
    std::vector<elementType> convert_sequence(sol::table t) {
        std::size_t sz = t.size();
        std::vector<elementType> res(sz);
        for (int i = 1; i <= sz; i++) {
            res[i - 1] = t[i];
        }
        return res;
    }
}

void LuaBinding::BindLua() {
    //辅助函数
    {
        auto sol2_ns_table = sol_state_["sol2"].get_or_create<sol::table>();
        sol2_ns_table["convert_sequence_float"] = &sol2::convert_sequence<float>;
        sol2_ns_table["convert_sequence_ushort"] = &sol2::convert_sequence<unsigned short>;
        sol2_ns_table["convert_sequence_uchar"] = &sol2::convert_sequence<unsigned char>;
        sol2_ns_table["convert_sequence_int"] = &sol2::convert_sequence<int>;
    }
    //绑定glm::vec3
    {
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
        glm_ns_table.new_usertype<glm::vec3>("vec3", sol::call_constructor,
                                             sol::constructors<glm::vec3(const float &, const float &,
                                                                         const float &)>(),
                                             "x", &glm::vec3::x,
                                             "y", &glm::vec3::y,
                                             "z", &glm::vec3::z,
                                             "r", &glm::vec3::r,
                                             "g", &glm::vec3::g,
                                             "b", &glm::vec3::b,
                                             sol::meta_function::to_string,
                                             [](const glm::vec3 *vec) -> std::string { return glm::to_string(*vec); },
                                             sol::meta_function::addition,
                                             [](const glm::vec3 *vec_a, const glm::vec3 *vec_b) {
                                                 return (*vec_a) + (*vec_b);
                                             },
                                             sol::meta_function::subtraction,
                                             [](const glm::vec3 *vec_a, const glm::vec3 *vec_b) {
                                                 return (*vec_a) - (*vec_b);
                                             },
                                             sol::meta_function::multiplication,
                                             [](const glm::vec3 *vec, const float a) { return (*vec) * a; },
                                             sol::meta_function::division,
                                             [](const glm::vec3 *vec, const float a) { return (*vec) / a; },
                                             sol::meta_function::unary_minus,
                                             [](const glm::vec3 *vec) { return (*vec) * -1; },
                                             sol::meta_function::equal_to,
                                             [](const glm::vec3 *vec_a, const glm::vec3 *vec_b) {
                                                 return (*vec_a) == (*vec_b);
                                             }
        );
    }

    //绑定glm::vec4
    {
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
        glm_ns_table.new_usertype<glm::vec4>("vec4", sol::call_constructor,
                                             sol::constructors<glm::vec4(const float &, const float &, const float &,
                                                                         const float &)>(),
                                             "x", &glm::vec4::x,
                                             "y", &glm::vec4::y,
                                             "z", &glm::vec4::z,
                                             "w", &glm::vec4::w,
                                             "r", &glm::vec4::r,
                                             "g", &glm::vec4::g,
                                             "b", &glm::vec4::b,
                                             "a", &glm::vec4::a,
                                             sol::meta_function::to_string,
                                             [](const glm::vec4 *vec) { return glm::to_string(*vec); },
                                             sol::meta_function::addition,
                                             [](const glm::vec4 *vec_a, const glm::vec4 *vec_b) {
                                                 return (*vec_a) + (*vec_b);
                                             },
                                             sol::meta_function::subtraction,
                                             [](const glm::vec4 *vec_a, const glm::vec4 *vec_b) {
                                                 return (*vec_a) - (*vec_b);
                                             },
                                             sol::meta_function::multiplication,
                                             [](const glm::vec4 *vec, const float a) { return (*vec) * a; },
                                             sol::meta_function::division,
                                             [](const glm::vec4 *vec, const float a) { return (*vec) / a; },
                                             sol::meta_function::unary_minus,
                                             [](const glm::vec4 *vec) { return (*vec) * -1; },
                                             sol::meta_function::equal_to,
                                             [](const glm::vec4 *vec_a, const glm::vec4 *vec_b) {
                                                 return (*vec_a) == (*vec_b);
                                             }
        );
    }

    //绑定glm::mat4
    {
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
        glm_ns_table.new_usertype<glm::mat4>("mat4", sol::call_constructor,
                                             sol::constructors<glm::mat4(const float &)>(),
                                             sol::meta_function::to_string,
                                             [](const glm::mat4 *m) { return glm::to_string(*m); },
                                             sol::meta_function::addition,
                                             [](const glm::mat4 *m_a, const glm::mat4 *m_b) { return (*m_a) + (*m_b); },
                                             sol::meta_function::subtraction,
                                             [](const glm::mat4 *m_a, const glm::mat4 *m_b) { return (*m_a) - (*m_b); },
                                             sol::meta_function::multiplication,
                                             [](const glm::mat4 *m, const glm::vec4 *v) { return (*m) * (*v); },
                                             sol::meta_function::division,
                                             [](const glm::mat4 *m, const float a) { return (*m) / a; },
                                             sol::meta_function::unary_minus,
                                             [](const glm::mat4 *m) { return (*m) * -1; },
                                             sol::meta_function::equal_to,
                                             [](const glm::mat4 *m_a, const glm::mat4 *m_b) { return (*m_a) == (*m_b); }
        );
    }

    //绑定glm函数
    {
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
        glm_ns_table.set_function("rotate", sol::overload(
                [](const glm::mat4 *m, const float f, const glm::vec3 *v) { return glm::rotate(*m, f, *v); }));
        glm_ns_table.set_function("radians", sol::overload([](const float f) { return glm::radians(f); }));
        glm_ns_table.set_function("to_string", sol::overload(
                [](const glm::mat4 *m) { return glm::to_string((*m)); },
                [](const glm::vec3 *v) { return glm::to_string((*v)); }
        ));
    }

    auto cpp_ns_table = sol_state_["Cpp"].get_or_create<sol::table>();

    // component
    {
        cpp_ns_table.new_usertype<GameObject>("GameObject", sol::call_constructor,
                                              sol::constructors<GameObject(std::string)>(),
                                              "name", &GameObject::name,
                                              "set_name", &GameObject::set_name,
                                              "layer", &GameObject::layer,
                                              "set_layer", &GameObject::set_layer,
                                              "SetParent", &GameObject::SetParent,
                                              "ForeachComponent", &GameObject::ForeachComponent,
                                              "Foreach", &GameObject::Foreach,
                                              "AttachComponent", &GameObject::AttachComponent
        );

        cpp_ns_table.new_usertype<Component>("Component", sol::call_constructor, sol::constructors<Component()>(),
                                             "Awake", &Component::Awake,
                                             "Update", &Component::Update,
                                             "game_object", &Component::game_object,
                                             "set_game_object", &Component::set_game_object,
                                             "set_lua_component_instance", &Component::set_lua_component_instance
        );

        cpp_ns_table.new_usertype<Transform>("Transform", sol::call_constructor, sol::constructors<Transform()>(),
                                             sol::base_classes, sol::bases<Component>(),
                                             "position", &Transform::position,
                                             "rotation", &Transform::rotation,
                                             "scale", &Transform::scale,
                                             "set_position", &Transform::set_position,
                                             "set_rotation", &Transform::set_rotation,
                                             "set_scale", &Transform::set_scale
        );
    }

    // renderer
    {
        cpp_ns_table.new_usertype<Camera>("Camera", sol::call_constructor, sol::constructors<Camera()>(),
                                          sol::base_classes, sol::bases<Component>(),
                                          "SetView", &Camera::SetView,
                                          "SetPerspective", &Camera::SetPerspective,
                                          "SetOrthographic", &Camera::SetOrthographic,
                                          "view_mat4", &Camera::view_mat4,
                                          "projection_mat4", &Camera::projection_mat4,
                                          "set_clear_color", &Camera::set_clear_color,
                                          "set_clear_flag", &Camera::set_clear_flag,
                                          "Clear", &Camera::Clear,
                                          "depth", &Camera::depth,
                                          "set_depth", &Camera::set_depth,
                                          "culling_mask", &Camera::culling_mask,
                                          "set_culling_mask", &Camera::set_culling_mask,
                                          "Foreach", &Camera::Foreach,
                                          "current_camera", &Camera::current_camera,
                                          "Sort", &Camera::Sort
        );

        cpp_ns_table.new_usertype<Material>("Material", sol::call_constructor, sol::constructors<Material()>(),
                                            "Load", &Material::Load,
                                            "SetUniform1i", &Material::SetUniform1i
        );

        cpp_ns_table.new_usertype<MeshFilter>("MeshFilter", sol::call_constructor, sol::constructors<MeshFilter()>(),
                                              sol::base_classes, sol::bases<Component>(),
                                              "LoadMesh", &MeshFilter::LoadMesh,
                                              "GetMeshName", &MeshFilter::GetMeshName
        );

        cpp_ns_table.new_usertype<MeshRenderer>("MeshRenderer", sol::call_constructor,
                                                sol::constructors<MeshRenderer()>(),
                                                sol::base_classes, sol::bases<Component>(),
                                                "SetMaterial", &MeshRenderer::SetMaterial,
                                                "material", &MeshRenderer::material,
                                                "Render", &MeshRenderer::Render
        );

        cpp_ns_table.new_usertype<Shader>("Shader", sol::call_constructor, sol::constructors<Shader()>(),
                                          "Use", &Shader::Use,
                                          "Disuse", &Shader::Disuse,
                                          "program_id", &Shader::program_id,
                                          "Find", &Shader::Find
        );

//        cpp_ns_table.new_usertype<Texture2D>("Texture2D",
//                                             "texture_id", &Texture2D::texture_id,
//                                             "LoadFromFile", &Texture2D::LoadFromFile
//        );
    }

    // framework
    {
        cpp_ns_table.new_usertype<Application>("Application",
                                               "set_title", &Application::set_title,
                                               "set_data_path", &Application::set_data_path,
                                               "Init", &Application::Init,
                                               "Run", &Application::Run,
                                               "UpdateScreenSize", &Application::UpdateScreenSize,
                                               "Update", &Application::Update,
                                               "Render", &Application::Render
        );
    }

    //helpers
    {
        cpp_ns_table.new_usertype<Screen>("Screen",
                                          "width",&Screen::width,
                                          "height",&Screen::height,
                                          "aspect_ratio",&Screen::aspect_ratio,
                                          "set_width",&Screen::set_width,
                                          "set_height",&Screen::set_height,
                                          "set_width_height",&Screen::set_width_height
        );
    }
}

void LuaBinding::RunLuaFile(const std::string &script_file_path) {
    auto result = sol_state_.script_file(script_file_path);
    if (!result.valid()) {
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- LOAD LUA ERROR ----\n{}\n------------------------", err.what());
    }
}

sol::protected_function_result LuaBinding::CallLuaFunction() {
    sol::protected_function main_function = sol_state_["main"];
    sol::protected_function_result result = main_function();
    if (!result.valid()) {
        sol::error err = result;
//        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\n{}\n------------------------",err.what());
    }
    return result;
}
