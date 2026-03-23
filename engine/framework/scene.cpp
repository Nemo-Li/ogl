#include "scene.h"
#include <algorithm>
#include "helpers/aabb.h"

Scene::Scene() {
    // 初始世界包围盒建议根据场景需要动态设置或作为参数传入
    octree_ = std::make_unique<Octree>(AABB(glm::vec3(-1000, -1000, -1000), glm::vec3(1000, 1000, 1000)));
}

Scene::~Scene() {
    // 析构时需要清理场景内所有的 GameObject
    for (auto* go : game_objects_) {
        delete go;
    }
    game_objects_.clear();
    always_visible_objects_.clear();
}

void Scene::AddGameObject(GameObject* gameObject) {
    if (std::find(game_objects_.begin(), game_objects_.end(), gameObject) == game_objects_.end()) {
        game_objects_.push_back(gameObject);
        octree_->Insert(gameObject);
    }
}

void Scene::RemoveGameObject(GameObject* gameObject) {
    auto it = std::find(game_objects_.begin(), game_objects_.end(), gameObject);
    if (it != game_objects_.end()) {
        game_objects_.erase(it);
        octree_->Remove(gameObject);
    }
    
    // 同时从 always_visible 列表中移除
    auto it_vis = std::find(always_visible_objects_.begin(), always_visible_objects_.end(), gameObject);
    if (it_vis != always_visible_objects_.end()) {
        always_visible_objects_.erase(it_vis);
    }
}

void Scene::UpdateGameObject(GameObject* gameObject) {
    octree_->Update(gameObject);
}

void Scene::GetVisibleObjects(const Frustum& frustum, std::vector<GameObject*>& results) const {
    // 1. 先加入始终可见的物体
    for (auto* go : always_visible_objects_) {
        if (go->active()) {
            results.push_back(go);
        }
    }

    // 2. 从八叉树中剔除并收集可见物体
    octree_->GetVisibleObjects([&frustum](const AABB& aabb) {
        return frustum.IsBoxVisible(aabb);
    }, results);
}

void Scene::SetAlwaysVisible(GameObject* gameObject, bool alwaysVisible) {
    auto it = std::find(always_visible_objects_.begin(), always_visible_objects_.end(), gameObject);
    if (alwaysVisible) {
        if (it == always_visible_objects_.end()) {
            always_visible_objects_.push_back(gameObject);
            // 既然是始终可见，可以从八叉树移除，避免重复计算
            octree_->Remove(gameObject);
        }
    } else {
        if (it != always_visible_objects_.end()) {
            always_visible_objects_.erase(it);
            // 重新加入八叉树以便正常剔除
            octree_->Insert(gameObject);
        }
    }
}
