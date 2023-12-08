require("lua_extension")

Screen = {

}

function Screen:width()
    return Cpp.Screen.width()
end

function Screen:height()
    return Cpp.Screen.height()
end

--- 获取屏幕宽高比
function Screen:aspect_ratio()
    return Cpp.Screen.aspect_ratio()
end

function Screen:set_width()
    Cpp.Screen.set_width()
end

function Screen:set_height()
    Cpp.Screen.set_height()
end

function Screen:set_width_height()
    Cpp.Screen.set_width_height()
end