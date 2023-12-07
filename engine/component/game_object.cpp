//
// Created by Nemo li on 2023/12/6.
//

#include "game_object.h"
#include "component.h"
#include <utility>

GameObject::GameObject() {

}

GameObject::GameObject(std::string name) {
    set_name(std::move(name));
}

GameObject::~GameObject() {

}

Component *GameObject::AddComponent(std::string component_type_name) {
    rttr::type t = rttr::type::get_by_name(component_type_name);
    // 创建实例
    rttr::variant var = t.create();
    Component *component = var.get_value<Component *>();
    component->set_game_object(this);

    if (component_type_instance_map_.find(component_type_name) == component_type_instance_map_.end()) {
        std::vector<Component *> component_vec;
        component_vec.push_back(component);
        component_type_instance_map_[component_type_name] = component_vec;
    } else {
        component_type_instance_map_[component_type_name].push_back(component);
    }
    return component;
}

Component *GameObject::GetComponent(std::string component_type_name) {
    if (component_type_instance_map_.find(component_type_name) == component_type_instance_map_.end()) {
        return nullptr;
    }
    if (component_type_instance_map_[component_type_name].size() == 0) {
        return nullptr;
    }
    return component_type_instance_map_[component_type_name][0];
}

std::vector<Component *> &GameObject::GetComponents(std::string component_type_name) {
    return component_type_instance_map_[component_type_name];
}
