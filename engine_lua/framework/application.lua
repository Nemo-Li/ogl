require("lua_extension")

Application={

}

function Application:Init()
    Cpp.Application.Init()
end

--- 开始引擎主循环
function Application:Run()
    Cpp.Application.Run()
end

--- 设置标题
--- @param title string @标题
function Application:set_title(title)
    Cpp.Application.set_title(title)
end

--- 设置资源目录
--- @param data_path string @资源目录
function Application:set_data_path(data_path)
    Cpp.Application.set_data_path(data_path)
end

function Application:UpdateScreenSize()
    return Cpp.Application.UpdateScreenSize()
end