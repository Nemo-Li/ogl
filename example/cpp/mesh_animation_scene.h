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
    Transform *player_transform;
    Transform *camera_transform;
    GameObject *go_skeleton;
    GameObject *go_skybox;
    GameObject *go_terrain;
    GameObject *go_sponza;
    Terrain *terrain;

    glm::vec4 cam_forward = glm::vec4(0.0, 0.0, -1.0, 1.0);
    glm::vec4 cam_left = glm::vec4(-1.0, 0.0, 0.0, 1.0);
    glm::mat4 unit_mat4 = glm::mat4(1.0f);

    //-z 方向的夹角 前进方向的夹角
    float degrees_step = 1.0f;

    void Awake() override {
        //创建主相机游戏对象
        auto *go_camera = new GameObject("main_camera");
        //挂上Transform 组件
        camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 0, 5));
        //挂上Camera 组件
        camera = go_camera->AddComponent<Camera>();
        camera->set_depth(0);
        camera->SetPerspective(60, Screen::aspect_ratio(), 0.01, 1000);

        go_skeleton = new GameObject("skeleton");
        player_transform = go_skeleton->AddComponent<Transform>();
        player_transform->set_scale(glm::vec3(2, 2, 2));
        player_transform->set_rotation(glm::vec3(-90, 0, 0));

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

        player_transform->set_position(glm::vec3(0, terrain->GetElevation(0, -2), -2));

        go_sponza = new GameObject("sponza");
        Transform *sponza_transform = go_sponza->AddComponent<Transform>();
        sponza_transform->set_scale(glm::vec3(0.09, 0.09, 0.09));
        sponza_transform->set_position(glm::vec3(10, 15, 0));
        MeshFilter *sponza_mesh_filter = go_sponza->AddComponent<MeshFilter>();
        Model *model = new Model("model/tank/tank", 3);
//        Model *model = new Model("model/tree/tree", 2);
//        Model *model = new Model("model/sponza/sponza", 2);
        model->shader = Shader::Find("shader/phong");
        sponza_mesh_filter->SetModel(model);

        MeshRenderer *meshRender = go_sponza->AddComponent<MeshRenderer>();
    }

    void Update() override {
        glm::vec3 old_pos = camera_transform->position();
        glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y, old_pos.z);

        //旋转相机
        if (Input::GetKeyDown(KEY_CODE_RIGHT) || Input::GetKeyDown(KEY_CODE_LEFT)) {
            glm::mat4 r = glm::rotate(unit_mat4, glm::radians(-degrees_step), glm::vec3(0, 1, 0));
            if (Input::GetKeyDown(KEY_CODE_LEFT)) {
                r = glm::rotate(unit_mat4, glm::radians(degrees_step), glm::vec3(0, 1, 0));
            }
            cam_forward = r * cam_forward;
            cam_left = r * cam_left;
        }

        if (Input::GetKeyDown(KEY_CODE_W)) {
            new_pos += glm::vec3(cam_forward);
        }

        if (Input::GetKeyDown(KEY_CODE_S)) {
            new_pos -= glm::vec3(cam_forward);
        }

        if (Input::GetKeyDown(KEY_CODE_A)) {
            new_pos += glm::vec3(cam_left);
        }

        if (Input::GetKeyDown(KEY_CODE_D)) {
            new_pos -= glm::vec3(cam_left);
        }

        float elevation = terrain->GetElevation(new_pos.x, new_pos.z);
        new_pos.y = elevation + 5.0f;
        camera_transform->set_position(new_pos);
        camera->SetView(glm::vec3(cam_forward), glm::vec3(0.0, 1.0, 0.0));
    }
};

#endif //OGL_SKIN_MESH_SCENE_H
