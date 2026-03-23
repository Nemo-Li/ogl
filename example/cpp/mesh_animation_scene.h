//
// Created by Nemo li on 2023/12/10.
//

#ifndef OGL_SKIN_MESH_SCENE_H
#define OGL_SKIN_MESH_SCENE_H

#include "component/component.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "renderer/camera.h"
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
#include "physics/rigid_body.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class MeshAnimationScene : public Component {
public:
    Camera *camera;
    Camera *debug_camera;
    Transform *camera_transform;
    Transform *player_transform;

    GameObject *go_skeleton;
    GameObject *go_skybox;
    GameObject *go_terrain;
    GameObject *go_tank;
    GameObject *go_house;
    GameObject *go_cottage;
    GameObject *go_ninja;
    Terrain *terrain;
    SkyBox *skyBox;
    Grass *grass;
    Light *light;
    AnimMeshRender *animRender;
    EffekseerEffect *effekseerEffect;

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
//        camera->set_culling_mask(0x00);

        //调试窗口相机
        // auto *go_debug_camera = new GameObject("debug_camera");
        // go_debug_camera->AddComponent<Transform>();
        // debug_camera = go_debug_camera->AddComponent<Camera>();
        // debug_camera->set_depth(1);
        // debug_camera->set_culling_mask(0x02);
        // debug_camera->set_clear_flag(GL_DEPTH_BUFFER_BIT);
        // debug_camera->SetOrthographic(-1.0f, 1.0f,
        //                               -1.0f,
        //                               1.0f, 0.01, 100);

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

        skyBox->LoadCubeMap("skybox/px.png", "skybox/nx.png",
                            "skybox/py.png", "skybox/ny.png",
                            "skybox/pz.png", "skybox/nz.png");

        go_terrain = new GameObject("terrain");
        terrain = go_terrain->AddComponent<Terrain>();
        auto *terrain_m = new Material();
        terrain_m->Load("material/terrain.mat");
        terrain->SetMaterial(terrain_m);
        terrain->LoadTerrainTexture("image/terrain.png");
        auto *terrainTrans = go_terrain->AddComponent<Transform>();
        
        // Add physics to terrain (using HeightfieldTerrainShape)
        auto* terrain_rb = go_terrain->AddComponent<RigidBody>();
        
        int terrainW = terrain->get_width();
        int terrainH = terrain->get_height();
        float minH = terrain->get_min_height();
        float maxH = terrain->get_max_height();
        const float* heightData = terrain->get_elevation_vec().data();

        // upAxis: 1 = Y axis
        // heightScale: 1.0f (already scaled in elevation_vec)
        // heightDataType: PHY_FLOAT
        // flipQuadEdges: false
        btHeightfieldTerrainShape* heightfieldShape = new btHeightfieldTerrainShape(
            terrainW, terrainH, heightData, 1.0f, minH, maxH, 1, PHY_FLOAT, false);
        
        // Bullet heightfield local origin is its center.
        // We need to move the collider to match our terrain's position.
        // Our terrain is centered in X-Z but Y is based on elevation values.
        // The midpoint of minHeight and maxHeight is where Bullet puts the local origin (y=0).
        float midH = (minH + maxH) / 2.0f;
        
        terrain_rb->Init(heightfieldShape, 0.0f); // mass 0 = static
        
        // Set the world position to match the terrain.
        // Note: Init uses the current transform position.
        // We need to adjust it for the midH offset that Bullet introduces.
        btTransform btTrans = terrain_rb->GetBtRigidBody()->getWorldTransform();
        btTrans.setOrigin(btVector3(0, midH, 0));
        terrain_rb->GetBtRigidBody()->setWorldTransform(btTrans);

        auto go_grass = new GameObject("grass");
        grass = go_grass->AddComponent<Grass>();
        auto *grass_m = new Material();
        grass_m->Load("material/grass.mat");
        grass->SetMaterial(grass_m);
        grass->LoadTerrainTexture("image/terrain.png");

        // Set NPC position before physics init
        npc_transform->set_position(glm::vec3(8, terrain->GetElevation(8, -2), -2));

        // Add physics to NPC
        auto* npc_rb = go_skeleton->AddComponent<RigidBody>();
        btCapsuleShape* npc_shape = new btCapsuleShape(2.0f, 8.0f); 
        npc_rb->Init(npc_shape, 1.0f);
        npc_rb->GetBtRigidBody()->setAngularFactor(btVector3(0, 0, 0));

        go_tank = new GameObject("tank");
        auto *tank_transform = go_tank->AddComponent<Transform>();
        tank_transform->set_scale(glm::vec3(0.5, 0.5, 0.5));
        tank_transform->set_position(glm::vec3(30, terrain->GetElevation(30, -20), -20));
        auto *tank_mesh_filter = go_tank->AddComponent<MeshFilter>();
        auto *model = new Model("model/tank/tank", 3);
        model->shader = Shader::Find("shader/phong");
        tank_mesh_filter->SetModel(model);
        go_tank->AddComponent<MeshRenderer>();

        // Add physics to tank
        auto* tank_rb = go_tank->AddComponent<RigidBody>();
        btBoxShape* tank_shape = new btBoxShape(btVector3(5, 5, 5));
        tank_rb->Init(tank_shape, 0.0f);

        go_house = new GameObject("house");
        auto *house_transform = go_house->AddComponent<Transform>();
        house_transform->set_scale(glm::vec3(5.0, 5.0, 5.0));
        house_transform->set_position(glm::vec3(0, terrain->GetElevation(0, -10), -10));
        auto house_mesh_filter = go_house->AddComponent<MeshFilter>();
        house_model = new Model("model/house/house", 2);
        house_model->shader = Shader::Find("shader/phong");
        house_mesh_filter->SetModel(house_model);
        go_house->AddComponent<MeshRenderer>();

        // Add physics to house
        // auto* house_rb = go_house->AddComponent<RigidBody>();
        // btBoxShape* house_shape = new btBoxShape(btVector3(15, 10, 15));
        // house_rb->Init(house_shape, 0.0f);

        go_cottage = new GameObject("cottage");
        auto *go_cottage_transform = go_cottage->AddComponent<Transform>();
        go_cottage_transform->set_scale(glm::vec3(1.0, 1.0, 1.0));
        go_cottage_transform->set_position(glm::vec3(0, terrain->GetElevation(0, 50), 50));
        auto cottage_mesh_filter = go_cottage->AddComponent<MeshFilter>();
        cottage_model = new Model("model/cottage_obj/cottage_obj", 2);
        cottage_model->shader = Shader::Find("shader/phong");
        cottage_mesh_filter->SetModel(cottage_model);
        go_cottage->AddComponent<MeshRenderer>();

        // Add physics to cottage
        // auto* cottage_rb = go_cottage->AddComponent<RigidBody>();
        // btBoxShape* cottage_shape = new btBoxShape(btVector3(10, 8, 10));
        // cottage_rb->Init(cottage_shape, 0.0f);

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

        go_ninja = new GameObject("ninja");
        AssLoader *assLoader = new AssLoader("model/ninja/ninja.mesh");
        player_transform = go_ninja->AddComponent<Transform>();
        player_transform->set_scale(glm::vec3(0.05, 0.05, 0.05));
        player_transform->set_position(glm::vec3(0, terrain->GetElevation(0, 10) + 1.1f, 10));
        animRender = go_ninja->AddComponent<AnimMeshRender>();
        animRender->SetAnimation(assLoader->assAnimation);

        // Add physics to player
        auto* player_rb = go_ninja->AddComponent<RigidBody>();
        // Use a capsule shape (height 2.0, radius 0.5)
        btCapsuleShape* player_shape = new btCapsuleShape(0.5f, 2.0f);
        player_rb->Init(player_shape, 1.0f);
        // Lock rotation for the player to keep them upright
        player_rb->GetBtRigidBody()->setAngularFactor(btVector3(0, 0, 0));

        auto go_effect = new GameObject("effect");
        effekseerEffect = go_effect->AddComponent<EffekseerEffect>();

        auto go_quad = new GameObject("quad");
        go_quad->set_layer(0x02);
        auto *quadTrans = go_quad->AddComponent<Transform>();
        quadTrans->set_rotation(glm::vec3(90, 0, 0));
//        quadTrans->set_position(glm::vec3(0.5, -0.5, 1));
        quadTrans->set_position(glm::vec3(0, 0, 1));
        auto *plane = new Plane(2, 2, 2);
        go_quad->AddComponent<MeshFilter>()->SetMesh(plane->mesh());
        auto *quad_m = new Material();
        quad_m->Load("material/mesh.mat");
        go_quad->AddComponent<MeshRenderer>()->SetMaterial(quad_m);
    }

    void runPlayerAction(int actionId) {
        animRender->curAnimation = actionId;
        if (animRender->playerActionEnd) {
            animRender->animStartTime = Time::TimeSinceStartup();
        }
    }

    void Update() override {
        glm::vec3 new_pos = glm::vec3(0, 0, 0); // This will store the desired velocity direction

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

        // Apply movement via physics
        auto* rb = go_ninja->GetComponent<RigidBody>();
        if (rb) {
            btRigidBody* bt_rb = rb->GetBtRigidBody();
            if (bt_rb) {
                if (glm::length(new_pos) > 0) {
                    new_pos = glm::normalize(new_pos);
                }
                
                // Keep the current Y velocity (for gravity) and only change X/Z
                btVector3 currentVel = bt_rb->getLinearVelocity();
                float speed = 10.0f; // Adjusted speed for physics
                bt_rb->setLinearVelocity(btVector3(new_pos.x * speed, currentVel.getY(), new_pos.z * speed));
                bt_rb->activate(true);
                
                // If no input, slow down X/Z movement (friction/damping)
                if (glm::length(new_pos) == 0) {
                    bt_rb->setLinearVelocity(btVector3(currentVel.getX() * 0.9f, currentVel.getY(), currentVel.getZ() * 0.9f));
                }
            }
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

        if (Input::GetKeyUp(KEY_CODE_SPACE)) {
            //runPlayerAction(3);
            //effekseerEffect->PlayEffect();
            
            // Apply jump impulse via physics
            auto* rb = go_ninja->GetComponent<RigidBody>();
            if (rb) {
                btRigidBody* bt_rb = rb->GetBtRigidBody();
                if (bt_rb) {
                    // Check if player is somewhat grounded before jumping (optional but better)
                    // For now, just apply an impulse
                    bt_rb->applyCentralImpulse(btVector3(0, 5.0, 0));
                    std::cout << "jump up" << std::endl;
                }
            }
        }

        if (Input::GetKeyDown(KEY_CODE_Z)) {
            y_offset--;
        }

        glm::vec3 player_pos = player_transform->position();
        glm::vec3 camera_pos = player_pos - 30.0f * glm::vec3(player_forward);
        camera_pos.y += y_offset;
        camera_transform->set_position(camera_pos);
        camera->SetView(glm::vec3(player_forward.x, player_forward.y + cam_pitch.y, player_forward.z),
                        glm::vec3(0.0, 1.0, 0.0));

        // Log player position
        std::cout << "Player Position: x=" << player_pos.x << ", y=" << player_pos.y << ", z=" << player_pos.z << std::endl;
    }
};

#endif //OGL_SKIN_MESH_SCENE_H
