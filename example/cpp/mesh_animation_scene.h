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
#include <mesh/plane.h>
#include "renderer/animation_clip.h"
#include "renderer/terrain.h"
#include <math.h>
#include <renderer/instance_render.h>
#include <renderer/grass.h>
#include <mesh/water.h>
#include <renderer/light.h>
#include <helpers/time.h>
#include "mesh/board.h"
#include "model/ass_loader.h"
#include "model/ass_model.h"
#include "renderer/anim_mesh_render.h"
#include "effect/effekseer_effect.h"

class MeshAnimationScene : public Component {
public:
    Camera *camera;
    Transform *camera_transform;
    Transform *player_transform;

    GameObject *go_skeleton;
    GameObject *go_skybox;
    GameObject *go_terrain;
    GameObject *go_tank;
    GameObject *go_house;
    GameObject *go_cottage;
    Terrain *terrain;
    SkyBox *skyBox;
    Grass *grass;
    Light *light;
    AnimMeshRender *animRender;

    Model *house_model;
    Model *cottage_model;

    glm::vec4 player_forward = glm::vec4(0.0, 0.0, -0.5, 1.0);
    glm::vec4 player_left = glm::vec4(-1.0, 0.0, 0.0, 1.0);
    glm::vec4 cam_pitch = glm::vec4(player_forward);
    glm::mat4 unit_mat4 = glm::mat4(1.0f);

    //-z 方向的夹角 前进方向的夹角
    float degrees_step = 1.0f;
    float y_offset = 10.0f;

    void Awake() override {
        //创建主相机游戏对象
        auto *go_camera = new GameObject("main_camera");
        //挂上Transform 组件
        camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 0, 0));
        //挂上Camera 组件
        camera = go_camera->AddComponent<Camera>();
        camera->set_depth(0);
        camera->SetPerspective(60, Screen::aspect_ratio(), 0.01, 1000);

        auto go_light = new GameObject("light");
        light = go_light->AddComponent<Light>();

        go_skeleton = new GameObject("skeleton");
        auto npc_transform = go_skeleton->AddComponent<Transform>();
        npc_transform->set_scale(glm::vec3(4, 4, 4));
        npc_transform->set_rotation(glm::vec3(-90, 0, 0));

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
        skyBox = go_skybox->AddComponent<SkyBox>();

        skyBox->LoadCubeMap("skybox/xpos.png", "skybox/xneg.png",
                            "skybox/ypos.png", "skybox/yneg.png",
                            "skybox/zpos.png", "skybox/zneg.png");

        go_terrain = new GameObject("terrain");
        terrain = go_terrain->AddComponent<Terrain>();
        auto *terrain_m = new Material();
        terrain_m->Load("material/terrain.mat");
        terrain->SetMaterial(terrain_m);
        terrain->LoadTerrainTexture("image/terrain.png");
        auto *terrainTrans = go_terrain->AddComponent<Transform>();

        auto go_grass = new GameObject("grass");
        grass = go_grass->AddComponent<Grass>();
        auto *grass_m = new Material();
        grass_m->Load("material/grass.mat");
        grass->SetMaterial(grass_m);
        grass->LoadTerrainTexture("image/terrain.png");

        npc_transform->set_position(glm::vec3(8, terrain->GetElevation(8, -2), -2));

        go_tank = new GameObject("tank");
        auto *tank_transform = go_tank->AddComponent<Transform>();
        tank_transform->set_scale(glm::vec3(0.5, 0.5, 0.5));
        tank_transform->set_position(glm::vec3(30, terrain->GetElevation(30, -20), -20));
        auto *tank_mesh_filter = go_tank->AddComponent<MeshFilter>();
        auto *model = new Model("model/tank/tank", 3);
        model->shader = Shader::Find("shader/phong");
        tank_mesh_filter->SetModel(model);
        go_tank->AddComponent<MeshRenderer>();

        go_house = new GameObject("house");
        auto *house_transform = go_house->AddComponent<Transform>();
        house_transform->set_scale(glm::vec3(5.0, 5.0, 5.0));
        house_transform->set_position(glm::vec3(0, terrain->GetElevation(0, -10), -10));
        auto house_mesh_filter = go_house->AddComponent<MeshFilter>();
        house_model = new Model("model/house/house", 2);
        house_model->shader = Shader::Find("shader/phong");
        house_mesh_filter->SetModel(house_model);
        go_house->AddComponent<MeshRenderer>();

        go_cottage = new GameObject("cottage");
        auto *go_cottage_transform = go_cottage->AddComponent<Transform>();
        go_cottage_transform->set_scale(glm::vec3(1.0, 1.0, 1.0));
        go_cottage_transform->set_position(glm::vec3(0, terrain->GetElevation(0, 50), 50));
        auto cottage_mesh_filter = go_cottage->AddComponent<MeshFilter>();
        cottage_model = new Model("model/cottage_obj/cottage_obj", 2);
        cottage_model->shader = Shader::Find("shader/phong");
        cottage_mesh_filter->SetModel(cottage_model);
        go_cottage->AddComponent<MeshRenderer>();

        auto go_tree = new GameObject("tree");
        auto *tree_m = new Material();
        tree_m->Load("material/tree.mat");
        auto instanceRender = go_tree->AddComponent<InstanceRender>();
        instanceRender->InitInstance(terrain);
        instanceRender->SetMaterial(tree_m);

        auto go_water = new GameObject("water");
        go_water->AddComponent<Transform>();
        auto water_mesh_filter = go_water->AddComponent<MeshFilter>();
        auto water = new Water(256, 16);
        water_mesh_filter->SetMesh(water->GetMesh());
        auto *water_m = new Material();
        water_m->Load("material/water.mat");
        go_water->AddComponent<MeshRenderer>()->SetMaterial(water_m);

        auto go_ninja = new GameObject("ninja");
        AssLoader *assLoader = new AssLoader("model/ninja/ninja.mesh");
        player_transform = go_ninja->AddComponent<Transform>();
        player_transform->set_scale(glm::vec3(0.05, 0.05, 0.05));
        player_transform->set_position(glm::vec3(0, terrain->GetElevation(0, 10), 10));
        animRender = go_ninja->AddComponent<AnimMeshRender>();
        animRender->SetAnimation(assLoader->assAnimation);

        auto go_effect = new GameObject("effect");
        go_effect->AddComponent<EffekseerEffect>();
    }

    void runPlayerAction(int actionId) {
        animRender->curAnimation = actionId;
        if (animRender->playerActionEnd) {
            animRender->animStartTime = Time::TimeSinceStartup();
        }
    }

    void Update() override {
        glm::vec3 old_pos = player_transform->position();
        glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y, old_pos.z);

        //旋转相机
        if (Input::GetKeyDown(KEY_CODE_RIGHT) || Input::GetKeyDown(KEY_CODE_LEFT)) {
            glm::mat4 r = glm::rotate(unit_mat4, glm::radians(-degrees_step), glm::vec3(0, 1, 0));
            glm::vec3 playerR = glm::vec3(0, 0, 0);
            if (Input::GetKeyDown(KEY_CODE_LEFT)) {
                r = glm::rotate(unit_mat4, glm::radians(degrees_step), glm::vec3(0, 1, 0));
                playerR = player_transform->rotation() + glm::vec3(0, degrees_step, 0);
                player_transform->set_rotation(playerR);
            } else {
                playerR = player_transform->rotation() - glm::vec3(0, degrees_step, 0);
                player_transform->set_rotation(playerR);
            }
            player_forward = r * player_forward;
            player_left = r * player_left;
            runPlayerAction(19);
        }

        //上下俯视仰视相机
        if (Input::GetKeyDown(KEY_CODE_DOWN) || Input::GetKeyDown(KEY_CODE_UP)) {
            glm::mat4 r = glm::rotate(unit_mat4, glm::radians(-degrees_step), glm::vec3(1, 0, 0));
            if (Input::GetKeyDown(KEY_CODE_UP)) {
                r = glm::rotate(unit_mat4, glm::radians(degrees_step), glm::vec3(1, 0, 0));
            }
            cam_pitch = r * cam_pitch;
        }

        if (Input::GetKeyDown(KEY_CODE_W)) {
            new_pos += glm::vec3(player_forward);
            runPlayerAction(19);
        }

        if (Input::GetKeyDown(KEY_CODE_S)) {
            new_pos -= glm::vec3(player_forward);
            runPlayerAction(19);
        }

        if (Input::GetKeyDown(KEY_CODE_A)) {
            new_pos += glm::vec3(player_left);
            runPlayerAction(19);
        }

        if (Input::GetKeyDown(KEY_CODE_D)) {
            new_pos -= glm::vec3(player_left);
            runPlayerAction(19);
        }

        if (Input::GetKeyUp(KEY_CODE_Q)) {
            float strength = light->lightStrength();
            printf("key q up\n");
            strength -= 0.1f;
            strength = std::max(strength, 0.1f);
            light->SetLightStrength(strength);
        }

        if (Input::GetKeyUp(KEY_CODE_E)) {
            float strength = light->lightStrength();
            strength += 0.1f;
            strength = std::min(strength, 1.0f);
            light->SetLightStrength(strength);
        }

        if (Input::GetKeyUp(KEY_CODE_X)) {
            light->stepRotate(-10);
        }

        if (Input::GetKeyUp(KEY_CODE_C)) {
            light->stepRotate(10);
        }

        if (Input::GetKeyUp(KEY_CODE_T)) {
            animRender->curAnimation++;
            if (animRender->curAnimation >= 20) {
                animRender->curAnimation = 0;
            }
        }

        if (Input::GetKeyDown(KEY_CODE_SPACE)) {
            runPlayerAction(3);
        }

        if (Input::GetKeyDown(KEY_CODE_Z)) {
            y_offset--;
        }

        float elevation = terrain->GetElevation(new_pos.x, new_pos.z);
        new_pos.y = elevation;
        player_transform->set_position(new_pos);

        glm::vec3 camera_pos = new_pos - 30 * glm::vec3(player_forward);
        camera_pos.y += y_offset;
        camera_transform->set_position(camera_pos);
        camera->SetView(glm::vec3(player_forward.x, player_forward.y + cam_pitch.y, player_forward.z),
                        glm::vec3(0.0, 1.0, 0.0));

    }
};

#endif //OGL_SKIN_MESH_SCENE_H
