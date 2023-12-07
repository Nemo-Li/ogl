//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_GAME_OBJECT_H
#define OGL_GAME_OBJECT_H

#include <string>
#include <unordered_map>
#include <utility>
#include <rttr/registration>

class Component;

class GameObject {
public:
    GameObject();

    GameObject(std::string name);

    virtual ~GameObject();

    std::string &name() { return name_; }

    void set_name(std::string name) { name_ = std::move(name); }

    /// 添加组件
    /// \param component_type_name 组件类名
    /// \return
    Component* AddComponent(std::string component_type_name);

    template<class T>
    T *AddComponent() {
        T *component = new T();
        rttr::type t = rttr::type::get(*component);
        std::string component_type_name = t.get_name().to_string();
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

    /// 获取组件
    /// \param component_type_name 组件类名
    /// \return
    Component* GetComponent(std::string component_type_name);

    /// 获取所有同名组件
    /// \param component_type_name 组件类名
    /// \return
    std::vector<Component*>& GetComponents(std::string component_type_name);

private:
    std::string name_;
    std::unordered_map<std::string, std::vector<Component *>> component_type_instance_map_;
};


#endif //OGL_GAME_OBJECT_H
