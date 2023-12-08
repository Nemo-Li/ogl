﻿Component = {}

function Component:ctor()
    self.game_object_ = nil
    self:InitCppComponent()
    self:SetToCpp()
end

--- 创建对应的C++实例
function Component:InitCppComponent()
    self.cpp_component_instance_ = Cpp.Component()
end

--- 将Lua Component绑定到C++端
function Component:SetToCpp()
    self.cpp_component_instance_:set_lua_component_instance(self)
end

--- 返回组件所属的GameObject
--- @return GameObject @返回组件所属的GameObject
function Component:game_object()
    return self.game_object_
end

function Component:set_game_object(game_object)
    self.game_object_ = game_object
    game_object:cpp_game_object_instance():AttachComponent(self.cpp_component_instance_)
    self.cpp_component_instance_:Awake()
end

function Component:OnEnable()

end

function Component:Awake()
    --print("Component:Awake")
end

function Component:Update()
    --print("Component:Update")
end

--- 渲染之前
function Component:OnPreRender()

end

--- 渲染之后
function Component:OnPostRender()

end

function Component:OnDisable()

end