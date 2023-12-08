﻿require("lua_extension")

--- 相机
--- @class Camera : CppComponent
Camera = class("Camera", Component)

function Camera:ctor()
    Camera.super.ctor(self)
end

function Camera:InitCppComponent()
    ---@type Cpp.Camera
    self.cpp_component_instance_ = Cpp.Camera() --创建对应的C++实例
end

--- 设置相机位置 朝向
--- @param camera_forward vec3 @相机朝前方向
--- @param camera_up vec3 @相机朝上方向
function Camera:SetView(camera_forward, camera_up)
    self.cpp_component_instance_:SetView(camera_forward, camera_up)
end

---@param fov_degrees number @相机视野 可视角度
---@param aspect_ratio number @宽高比
---@param near_clip number @近裁剪面
---@param far_clip number @远裁剪面
function Camera:SetPerspective(fov_degrees, aspect_ratio, near_clip, far_clip)
    self.cpp_component_instance_:SetPerspective(fov_degrees, aspect_ratio, near_clip, far_clip)
end

function Camera:view_mat4()
    return self.cpp_component_instance_:view_mat4()
end

function Camera:projection_mat4()
    return self.cpp_component_instance_:projection_mat4()
end

--- 设置正交相机矩阵
--- @param left number
--- @param right number
--- @param bottom number
--- @param top number
--- @param z_near number
--- @param z_far number
function Camera:SetOrthographic(left, right, bottom, top, z_near, z_far)
    self.cpp_component_instance_:SetOrthographic(left, right, bottom, top, z_near, z_far)
end

--- 设置清屏颜色
--- @param r number
--- @param g number
--- @param b number
--- @param a number
function Camera:set_clear_color(r, g, b, a)
    self.cpp_component_instance_:set_clear_color(r, g, b, a)
end

--- 设置刷帧清屏内容种类
--- @param clear_flag number
function Camera:set_clear_flag(clear_flag)
    self.cpp_component_instance_:set_clear_flag(clear_flag)
end

--- 刷帧清屏
function Camera:Clear()
    self.cpp_component_instance_:Clear()
end

function Camera:depth()
    return self.cpp_component_instance_:depth()
end

--- 设置 depth，触发相机排序
--- @param depth number @相机深度，多相机按从后往前排序，按depth从小往大遍历，depth越大的越上层。
function Camera:set_depth(depth)
    self.cpp_component_instance_:set_depth(depth)
end

function Camera:culling_mask()
    return self.cpp_component_instance_:culling_mask()
end

function Camera:set_culling_mask(culling_mask)
    self.cpp_component_instance_:set_culling_mask(culling_mask)
end

function Camera:camera_use_for()
    self.cpp_component_instance_:camera_use_for()
end