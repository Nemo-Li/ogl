require("helpers/debug")
require("framework/application")
require("component/game_object")
require("component/transform")
require("login_scene")

function main()
    Application:set_title("[load_mesh]")
    --设置资源目录
    Application:set_data_path("../res/")
    Application:Init()--初始化引擎

    local go = GameObject.new("LoginSceneGo")
    local transform = go:AddComponent(Transform)
    print("transform:" .. tostring(transform))
    local pos = transform:position()
    print("pos:" .. tostring(pos))

    go:AddComponent(LoginScene)

    --开始引擎主循环
    Application:Run()
end