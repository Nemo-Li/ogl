//
// Created by Nemo li on 2023/12/10.
//

#ifndef OGL_LOGIN_SCENE_H
#define OGL_LOGIN_SCENE_H

#include "component/component.h"

class LoginScene : public Component {

public:
    Camera *camera;
    Transform *transform;

    void Awake() override {
        //创建主相机
        GameObject *go_camera = new GameObject("main_camera");
        auto go_transform = go_camera->AddComponent<Transform>();
        go_transform->set_position(glm::vec3(0, 0, 5));
        camera = go_camera->AddComponent<Camera>();
        camera->set_depth(0);
        camera->SetView(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        camera->SetPerspective(60, Screen::aspect_ratio(), 1, 1000);

        //创建GameObject
        GameObject *go = new GameObject("fish_soup_pot");

        //挂上 Transform 组件
        transform = go->AddComponent<Transform>();
        transform->set_position(glm::vec3(0, -1, -5));

        //挂上 MeshFilter 组件
        auto mesh_filter = dynamic_cast<MeshFilter *>(go->AddComponent<MeshFilter>());
        mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

        //挂上MeshRenderer 组件
        auto mesh_renderer = dynamic_cast<MeshRenderer *>(go->AddComponent<MeshRenderer>());

        //设置材质
        Material *material = new Material();
        material->Load("material/fishsoup_pot.mat");
        mesh_renderer->SetMaterial(material);
    }

    void Update() override {
        //旋转物体
        if (Input::GetKeyDown(KEY_CODE_R)) {
            static float rotate_eulerAngle = 0.f;
            rotate_eulerAngle += 0.1f;
            glm::vec3 rotation = transform->rotation();
            rotation.y = rotate_eulerAngle;
            transform->set_rotation(rotation);
        }
    }
};

#endif //OGL_LOGIN_SCENE_H
