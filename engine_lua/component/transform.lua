require("lua_extension")

Transform=class("Transform",Component)

function Transform:ctor()
    Transform.super.ctor(self)
end

function Transform:InitCppComponent()
    ---@type Cpp.Transform
    self.cpp_component_instance_=Cpp.Transform()
end


function Transform:position()
    return self.cpp_component_instance_:position()
end

function Transform:rotation()
    return self.cpp_component_instance_:rotation()
end

function Transform:scale()
    return self.cpp_component_instance_:scale()
end

function Transform:set_position(position)
    return self.cpp_component_instance_:set_position(position)
end

function Transform:set_rotation(position)
    return self.cpp_component_instance_:set_rotation(position)
end

function Transform:set_scale(position)
    return self.cpp_component_instance_:set_scale(position)
end