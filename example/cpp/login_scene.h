//
// Created by Nemo li on 2023/12/10.
//

#ifndef OGL_LOGIN_SCENE_H
#define OGL_LOGIN_SCENE_H

#include "component/component.h"
#include "helpers/screen.h"
#include "renderer/mesh_filter.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"
#include "control/input.h"
#include "control/key_code.h"
#include <glm/glm.hpp>
#include <glm/gtx/scalar_multiplication.hpp>
#include <glm/gtx/string_cast.hpp>
#include "renderer/animation_clip.h"

class LoginScene : public Component {

public:
    Camera *camera;
    Transform *transform;
    Transform *camera_transform;
    AnimationClip *animation_clip;

    //上一帧的鼠标位置
    glm::vec2 last_frame_mouse_position = Input::mousePosition();

    void Awake() override {
        animation_clip = new AnimationClip();
        animation_clip->LoadFromFile("animation/export.skeleton_anim");
        animation_clip->Play();

        //创建主相机
        auto *go_camera = new GameObject("main_camera");
        camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 0, 5));
        camera = go_camera->AddComponent<Camera>();

        //创建GameObject
        auto *go = new GameObject("fish_soup_pot");

        //挂上 Transform 组件
        transform = go->AddComponent<Transform>();
        transform->set_position(glm::vec3(0, 0, 0));

        //挂上 MeshFilter 组件
        auto mesh_filter = dynamic_cast<MeshFilter *>(go->AddComponent<MeshFilter>());
        mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

        //挂上MeshRenderer 组件
        auto mesh_renderer = dynamic_cast<MeshRenderer *>(go->AddComponent<MeshRenderer>());

        //设置材质
        auto *material = new Material();
        material->Load("material/fishsoup_pot.mat");
        mesh_renderer->SetMaterial(material);
    }

    void Update() override {
        animation_clip->Update();
        //旋转物体
        if (Input::GetKeyDown(KEY_CODE_R)) {
            static float rotate_eulerAngle = 0.f;
            rotate_eulerAngle += 0.1f;
            glm::vec3 rotation = transform->rotation();
            rotation.y = rotate_eulerAngle;
            transform->set_rotation(rotation);
        }

        camera->set_depth(0);
        camera->SetView(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        camera->SetPerspective(60, Screen::aspect_ratio(), 1, 1000);

        //鼠标滚轮控制相机远近
        camera_transform->set_position(camera_transform->position() * (10 - Input::mouse_scroll()) / 10.f);

        //旋转相机
        if (Input::GetKeyDown(KEY_CODE_LEFT_ALT) && Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float degrees = Input::mousePosition().x - last_frame_mouse_position.x;
            glm::mat4 old_mat4 = glm::mat4(1.0f);
            std::cout << glm::to_string(old_mat4) << std::endl;
            //以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
            glm::mat4 rotate_mat4 = glm::rotate(old_mat4, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec4 old_pos = glm::vec4(camera_transform->position(), 1.0f);
            glm::vec4 new_pos = rotate_mat4 * old_pos;//旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
            std::cout << glm::to_string(new_pos) << std::endl;
            camera_transform->set_position(glm::vec3(new_pos));
        }
        last_frame_mouse_position = Input::mousePosition();
    }
};

#endif //OGL_LOGIN_SCENE_H
