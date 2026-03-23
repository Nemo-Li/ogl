#ifndef OGL_SCENE_H
#define OGL_SCENE_H

#include <vector>
#include <memory>
#include <functional>
#include "component/game_object.h"
#include "data_struct/octree.h"
#include "helpers/frustum.h"

class Scene {
public:
    Scene();
    ~Scene();

    /// 添加 GameObject 到场景
    void AddGameObject(GameObject* gameObject);
    /// 从场景移除 GameObject
    void RemoveGameObject(GameObject* gameObject);
    /// 更新场景中的 GameObject（如更新八叉树位置）
    void UpdateGameObject(GameObject* gameObject);

    /// 获取场景中的所有 GameObject（慎用，主要用于全局操作）
    const std::vector<GameObject*>& game_objects() const { return game_objects_; }

    /// 获取始终可见的物体（跳过剔除逻辑，如天空盒）
    const std::vector<GameObject*>& always_visible_objects() const { return always_visible_objects_; }

    /// 获取指定视锥体内的可见物体
    void GetVisibleObjects(const Frustum& frustum, std::vector<GameObject*>& results) const;

    /// 标记物体为始终可见
    void SetAlwaysVisible(GameObject* gameObject, bool alwaysVisible);

    /// 获取场景八叉树
    Octree* octree() { return octree_.get(); }

private:
    std::vector<GameObject*> game_objects_;
    std::vector<GameObject*> always_visible_objects_;
    std::unique_ptr<Octree> octree_;
};

#endif //OGL_SCENE_H
