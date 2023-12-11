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

class SkinMeshScene : public Component {
public:
    Camera *camera;
    Transform *transform;
    Transform *camera_transform;
    GameObject *go_skeleton;

    void Awake() override {
        float vertex_data[] = {-0.2, 0, 0, 1.0, 1.0, 1.0, 1.0, 0, 0,
                               0.2, 0, 0, 1.0, 1.0, 1.0, 1.0, 1, 0,
                               0.2, 2, 0, 1.0, 1.0, 1.0, 1.0, 1, 1,
                               -0.2, 2, 0, 1.0, 1.0, 1.0, 1.0, 0, 1,
                               -0.2, 2, 0, 1.0, 1.0, 1.0, 1.0, 0, 0,
                               0.2, 2, 0, 1.0, 1.0, 1.0, 1.0, 1, 0,
                               0.2, 3, 0, 1.0, 1.0, 1.0, 1.0, 1, 1,
                               -0.2, 3, 0, 1.0, 1.0, 1.0, 1.0, 0, 1,};

        float vertex_index_data[] = {0, 1, 2,
                                     0, 2, 3,
                                     4, 5, 6,
                                     4, 6, 7};

        unsigned int vertex_relate_bone_index[] = {0, 0, 0, 0,
                                                   0, 0, 1, 1};

        char vertex_relate_bone_infos[] = {
                0, -1, -1, -1, 100, -1, -1, -1,
                0, -1, -1, -1, 100, -1, -1, -1,
                0, -1, -1, -1, 100, -1, -1, -1,
                0, -1, -1, -1, 100, -1, -1, -1,
                0, 1, -1, -1, 70, 30, -1, -1,
                0, -1, -1, -1, 100, -1, -1, -1,
                1, -1, -1, -1, 100, -1, -1, -1,
                1, -1, -1, -1, 100, -1, -1, -1,
        };

        std::vector<float> vertex_data_vec;
        vertex_data_vec.insert(vertex_data_vec.end(), &vertex_data[0], &vertex_data[0] + 72);

        std::vector<unsigned short> vertex_index_data_vec;
        vertex_index_data_vec.insert(vertex_index_data_vec.end(),
                                     &vertex_index_data[0], &vertex_index_data[0] + 12);

        std::vector<unsigned char> vertex_relate_bone_index_vec;
        vertex_relate_bone_index_vec.insert(vertex_relate_bone_index_vec.end(),
                                            &vertex_relate_bone_index[0], &vertex_relate_bone_index[0] + 8);

        std::vector<char> vertex_relate_bone_infos_vec;
        vertex_relate_bone_infos_vec.insert(vertex_relate_bone_infos_vec.end(),
                                            &vertex_relate_bone_infos[0], &vertex_relate_bone_infos[0] + 64);


        //创建主相机游戏对象
        auto *go_camera = new GameObject("main_camera");
        //挂上Transform 组件
        camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 0, 5));
        //挂上Camera 组件
        camera = go_camera->AddComponent<Camera>();

        go_skeleton = new GameObject("skeleton");
        transform = go_skeleton->AddComponent<Transform>();
        transform->set_position(glm::vec3(0, 0, 0));

        Animation *animation = go_skeleton->AddComponent<Animation>();
        animation->LoadAnimationClipFromFile("animation/export.skeleton_anim", "rotate");

        MeshFilter *meshFilter = go_skeleton->AddComponent<MeshFilter>();
        meshFilter->CreateMesh(vertex_data_vec, vertex_index_data_vec);
        meshFilter->LoadWeight("model/export.weight");

        Material *material = new Material();
        material->Load("material/cube.mat");

        SkinnedMeshRenderer *skinnedMeshRenderer = go_skeleton->AddComponent<SkinnedMeshRenderer>();
        skinnedMeshRenderer->SetMaterial(material);

        go_skeleton->GetComponent<Animation>()->Play("rotate");
    }

    void Update() override {
        camera->set_depth(0);
        camera->SetView(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        camera->SetPerspective(60, Screen::aspect_ratio(), 1, 1000);
    }

};

#endif //OGL_SKIN_MESH_SCENE_H
