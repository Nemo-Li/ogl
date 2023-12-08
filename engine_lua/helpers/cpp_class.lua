require("lua_extension")

--- @class CppClass @表示当前Lua Class，是对C++ Class的封装
CppClass = class("CppClass")

--- CppClass构造函数
--- @param cpp_class_instance table 对应C++端的Class实例。
function CppClass:ctor(cpp_class_instance)
    if cpp_class_instance then
        self.cpp_class_instance_ = cpp_class_instance
    else
        self:InitCppClass()
    end
end

--- 实例化C++ Class
function CppClass:InitCppClass()
    --- @public field cpp_class_instance_ table @C++ Class实例
    self.cpp_class_instance_ = nil
end

function CppClass:cpp_class_instance()
    return self.cpp_class_instance_
end