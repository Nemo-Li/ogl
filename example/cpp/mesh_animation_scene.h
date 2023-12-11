//
// Created by Nemo li on 2023/12/10.
//

#ifndef OGL_SKIN_MESH_SCENE_H
#define OGL_SKIN_MESH_SCENE_H

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
#include <renderer/animation.h>
#include <renderer/skinned_mesh_renderer.h>
#include "renderer/animation_clip.h"

class MeshAnimationScene : public Component {
public:
    Camera *camera;
    Transform *transform;
    Transform *camera_transform;
    GameObject *go_skeleton;

    //上一帧的鼠标位置
    glm::vec2 last_frame_mouse_position = Input::mousePosition();

    void Awake() override {
        //创建主相机游戏对象
        auto *go_camera = new GameObject("main_camera");
        //挂上Transform 组件
        camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 0, 5));
        //挂上Camera 组件
        camera = go_camera->AddComponent<Camera>();

        go_skeleton = new GameObject("skeleton");
        transform = go_skeleton->AddComponent<Transform>();
        transform->set_position(glm::vec3(0, -1, 0));
        transform->set_rotation(glm::vec3(-90, 0, 0));

        Animation *animation = go_skeleton->AddComponent<Animation>();
        animation->LoadAnimationClipFromFile("animation/fbx_extra_bip001_bip001_take_001_baselayer.skeleton_anim",
                                             "play");

        MeshFilter *meshFilter = go_skeleton->AddComponent<MeshFilter>();
        meshFilter->LoadMesh("model/fbx_extra_jiulian.mesh");
        meshFilter->LoadWeight("model/fbx_extra_jiulian.weight");

        Material *material = new Material();
        material->Load("material/fbx_extra_jiulian.mat");

        SkinnedMeshRenderer *skinnedMeshRenderer = go_skeleton->AddComponent<SkinnedMeshRenderer>();
        skinnedMeshRenderer->SetMaterial(material);

        go_skeleton->GetComponent<Animation>()->Play("play");
    }

    void Update() override {
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

#endif //OGL_SKIN_MESH_SCENE_H
