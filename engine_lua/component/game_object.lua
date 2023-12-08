﻿require("lua_extension")
require("component")

--- GameObject是游戏物体空节点，可以挂载多个组件
--- @class GameObject
GameObject=class("GameObject")

function GameObject:ctor(name)
    --- @type Component[] @组件列表
    self.components_map_={}
    --- @type Cpp.GameObject @C++ GameObject对象
    self.cpp_game_object_instance_=Cpp.GameObject(name)
end

function GameObject:cpp_game_object_instance()
    return self.cpp_game_object_instance_
end

--- @return string
function GameObject:name()
    return self.cpp_game_object_instance_:name()
end

--- @param name string
function GameObject:set_name(name)
    self.cpp_game_object_instance_:set_name(name)
end

--- @return number
function GameObject:layer()
    return self.cpp_game_object_instance_:layer()
end

--- @param layer number
function GameObject:set_layer(layer)
    self.cpp_game_object_instance_:set_layer(layer)
end

--- @return boolean
function GameObject:active()
    return self.cpp_game_object_instance_:active()
end

--- @param active boolean
function GameObject:set_active(active)
    self.cpp_game_object_instance_:set_active(active)
end

---@param component_type table @组件类型
---@return Component @组件
function GameObject:AddComponent(component_type)
    local component_instance=component_type.new()
    if self.components_map_[component_type]==nil then
        self.components_map_[component_type]={}
    end
    table.insert(self.components_map_[component_type],component_instance)
    component_instance:set_game_object(self)
    return component_instance
end

--- 从GameObject上获取组件
--- @param component_type table @组件类型
--- @return table @组件实例
function GameObject:GetComponent(component_type)
    if self.components_map_[component_type] and #self.components_map_[component_type]>0 then
        return self.components_map_[component_type][1]
    end
    return nil
end

--- 获取指定类型所有组件
--- @param component_type table @组件类型
--- @return table @指定类型所有组件
function GameObject:GetComponents(component_type)
    --print("GetComponents self.components_map_:" .. table_tostring(self.components_map_))
    local return_components={}
    for key,components in pairs(self.components_map_) do
        if key==component_type or is_sub_class_of(key,component_type) then
            --print("GetComponents check:" .. table_tostring(components))
            combine_list(return_components,components)
        end
    end
    return return_components
end