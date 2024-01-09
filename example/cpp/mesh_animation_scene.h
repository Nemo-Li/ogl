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
#include <glm/gtx/transform2.hpp>
#include <renderer/animation.h>
#include <renderer/skinned_mesh_renderer.h>
#include <renderer/sky_box.h>
#include <glm/gtx/euler_angles.hpp>
#include "renderer/animation_clip.h"
#include "renderer/terrain.h"

class MeshAnimationScene : public Component {
public:
    Camera *camera;
    Transform *transform;
    Transform *camera_transform;
    GameObject *go_skeleton;
    GameObject *go_skybox;
    GameObject *go_terrain;
    Terrain *terrain;
    glm::vec3 cam_forward = glm::vec3(0.0, 0.0, -1.0);

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

        go_skybox = new GameObject("skybox");
        SkyBox *skyBox = go_skybox->AddComponent<SkyBox>();
        skyBox->LoadCubeMap("skybox/sky_ft.png", "skybox/sky_bk.png",
                            "skybox/sky_dn.png", "skybox/sky_up.png",
                            "skybox/sky_rt.png", "skybox/sky_lf.png");

        go_terrain = new GameObject("terrain");
        terrain = go_terrain->AddComponent<Terrain>();
        auto *terrain_m = new Material();
        terrain_m->Load("material/terrain.mat");
        terrain->SetMaterial(terrain_m);
        terrain->LoadTerrainTexture("image/terrain1024.png");
        auto *terrainTrans = go_terrain->AddComponent<Transform>();
//        terrainTrans->set_rotation(glm::vec3(0, 0, 180));
    }

    void Update() override {
        camera->set_depth(0);
        camera->SetView(cam_forward, glm::vec3(0.0, 1.0, 0.0));
        camera->SetPerspective(60, Screen::aspect_ratio(), 0.01, 1000);

        //鼠标滚轮控制相机远近
        camera_transform->set_position(camera_transform->position() * (10 - Input::mouse_scroll()) / 10.f);

        //旋转相机
        if (Input::GetKeyDown(KEY_CODE_LEFT_ALT) && Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float degrees = Input::mousePosition().x - last_frame_mouse_position.x;
            glm::mat4 old_mat4 = glm::mat4(1.0f);
            //以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
//            glm::mat4 rotate_mat4 = glm::rotate(old_mat4, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 t1 = glm::translate(old_mat4, -camera_transform->position());
            glm::mat4 r = glm::rotate(old_mat4, glm::radians(degrees), glm::vec3(0, 1, 0));
            glm::mat4 t2 = glm::translate(old_mat4, camera_transform->position());
            glm::vec4 cam_forward_vec4 = glm::vec4(cam_forward, 1.0f);
            //旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
            glm::vec4 new_pos = t2 * r * t1 * cam_forward_vec4;
            cam_forward = glm::vec3(new_pos);
        }
        last_frame_mouse_position = Input::mousePosition();

        glm::vec3 old_pos = camera_transform->position();
        glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y, old_pos.z);

        if (Input::GetKeyDown(KEY_CODE_W)) {
            new_pos.z -= 1.0f;
        }

        if (Input::GetKeyDown(KEY_CODE_S)) {
            new_pos.z += 1.0f;
        }

        if (Input::GetKeyDown(KEY_CODE_A)) {
            new_pos.x -= 1.0f;
            printf("camera position x z y %f %f %f\n", old_pos.x, old_pos.z, old_pos.y);
        }

        if (Input::GetKeyDown(KEY_CODE_D)) {
            new_pos.x += 1.0f;
        }

        float elevation = terrain->GetElevation(new_pos.x, new_pos.z);
        new_pos.y = elevation + 5.0f;
        camera_transform->set_position(new_pos);
    }

};

#endif //OGL_SKIN_MESH_SCENE_H
