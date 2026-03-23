#ifndef OGL_PHYSICS_SCENE_H
#define OGL_PHYSICS_SCENE_H

#include "component/component.h"
#include "helpers/screen.h"
#include "renderer/mesh_filter.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"
#include "control/input.h"
#include "control/key_code.h"
#include "physics/rigid_body.h"
#include <glm/glm.hpp>

class PhysicsScene : public Component {
public:
    void Awake() override {
        // Create Camera
        auto *go_camera = new GameObject("main_camera");
        auto* camera_transform = go_camera->AddComponent<Transform>();
        camera_transform->set_position(glm::vec3(0, 10, 10));
        auto* camera = go_camera->AddComponent<Camera>();
        camera->set_depth(0);
        camera->SetView(glm::vec3(0.0, -1.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
        camera->SetPerspective(60, Screen::aspect_ratio(), 0.01, 1000);

        // Create Ground
        // auto *go_ground = new GameObject("ground");
        // auto* ground_transform = go_ground->AddComponent<Transform>();
        // ground_transform->set_position(glm::vec3(0, 0, 0));
        // ground_transform->set_scale(glm::vec3(10, 0.1f, 10));

        // auto* ground_mesh_filter = go_ground->AddComponent<MeshFilter>();
        // ground_mesh_filter->LoadMesh("model/tree/tree.obj"); // Just use any mesh for now

        // auto* ground_mesh_renderer = go_ground->AddComponent<MeshRenderer>();
        // auto* ground_material = new Material();
        // ground_material->Load("material/mesh.mat");
        // ground_mesh_renderer->SetMaterial(ground_material);

        // auto* ground_rb = go_ground->AddComponent<RigidBody>();
        // btBoxShape* ground_shape = new btBoxShape(btVector3(10, 0.1f, 10));
        // ground_rb->Init(ground_shape, 0.0f); // mass 0 = static


        auto go_quad = new GameObject("quad");
        auto *quadTrans = go_quad->AddComponent<Transform>();
        quadTrans->set_position(glm::vec3(0, 0, 0));
        auto *plane = new Plane(8, 8, 8);
        go_quad->AddComponent<MeshFilter>()->SetMesh(plane->mesh());
        auto *quad_m = new Material();
        quad_m->Load("material/mesh.mat");
        go_quad->AddComponent<MeshRenderer>()->SetMaterial(quad_m);
        auto* quad_rb = go_quad->AddComponent<RigidBody>();
        btBoxShape* quad_shape = new btBoxShape(btVector3(8, 0.1, 8));
        quad_rb->Init(quad_shape, 0.0f);

        // auto* go_terrain = new GameObject("terrain");
        // auto* terrain = go_terrain->AddComponent<Terrain>();
        // auto* terrain_m = new Material();
        // terrain_m->Load("material/terrain.mat");
        // terrain->SetMaterial(terrain_m);
        // terrain->LoadTerrainTexture("image/terrain.png");
        // auto* terrainTrans = go_terrain->AddComponent<Transform>();

        // Create Falling Box
        auto *go_box = new GameObject("falling_box");
        auto* box_transform = go_box->AddComponent<Transform>();
        box_transform->set_position(glm::vec3(0, 10, 0));
        box_transform->set_scale(glm::vec3(1, 1, 1));
        auto *box = new Plane(3, 3, 3);
        go_box->AddComponent<MeshFilter>()->SetMesh(box->mesh());
        auto* box_material = new Material();
        box_material->Load("material/mesh.mat");
        go_box->AddComponent<MeshRenderer>()->SetMaterial(box_material);

        auto* box_rb = go_box->AddComponent<RigidBody>();
        btBoxShape* box_shape = new btBoxShape(btVector3(1, 1, 1));
        box_rb->Init(box_shape, 1.0f);
    }

    void Update() override {
        // Basic camera controls could be here
    }
};

#endif //OGL_PHYSICS_SCENE_H
