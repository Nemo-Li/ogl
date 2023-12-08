require("lua_extension")
require("renderer/camera")
require("renderer/mesh_filter")
require("renderer/mesh_renderer")
require("renderer/material")
require("helpers/screen")

LoginScene = class("LoginScene", Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    self.go_camera_ = nil
    ---@type Camera
    self.camera_ = nil --相机
    self.go_mesh_ = nil --物体
    self.material_ = nil --材质
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)
    --创建相机1 GameObject
    self.go_camera_ = GameObject.new("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent(Transform):set_position(glm.vec3(0, 0, 5))
    --挂上 Camera 组件
    self.camera_ = self.go_camera_:AddComponent(Camera)
    self.camera_:set_depth(0)

    --创建骨骼蒙皮动画
    self.go_mesh_ = GameObject.new("mesh")
    self.go_mesh_:AddComponent(Transform):set_position(glm.vec3(0, -1, 0))
    self.go_mesh_:GetComponent(Transform):set_rotation(glm.vec3(-90, 0, 0))

    local mesh_filter = self.go_mesh_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/fbx_extra_jiulian.mesh")--加载Mesh

    --手动创建Material
    self.material_ = Material.new()--设置材质
    self.material_:Load("material/fbx_extra_jiulian.mat")

    --挂上 MeshRenderer 组件
    local mesh_renderer = self.go_mesh_:AddComponent(MeshRenderer)
    mesh_renderer:SetMaterial(self.material_)
end

function LoginScene:Update()
    --print("LoginScene:Update")
    self.camera_:set_depth(0)
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0, 0.0, 0.0), glm.vec3(0.0, 1.0, 0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)
    --鼠标滚轮控制相机远近
    --self.go_camera_:GetComponent(Transform):set_position(self.go_camera_:GetComponent(Transform):position())
end