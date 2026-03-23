#ifndef OGL_OCTREE_H
#define OGL_OCTREE_H

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include "helpers/aabb.h"

class GameObject;

class OctreeNode {
public:
    static const int MAX_OBJECTS_PER_NODE = 8;
    static const int MAX_DEPTH = 6;

    OctreeNode(const AABB& bounds, int depth = 0);
    ~OctreeNode();

    void Insert(GameObject* object, const AABB& objectBounds);
    void Remove(GameObject* object);
    void Update(GameObject* object, const AABB& oldBounds, const AABB& newBounds);

    void GetObjectsInFrustum(const std::function<bool(const AABB&)>& frustumCheck, std::vector<GameObject*>& results) const;

private:
    void Subdivide();
    bool IsLeaf() const { return children_[0] == nullptr; }

    AABB bounds_;
    int depth_;
    std::vector<std::pair<GameObject*, AABB>> objects_;
    std::unique_ptr<OctreeNode> children_[8];
};

class Octree {
public:
    Octree(const AABB& worldBounds);
    
    void Insert(GameObject* object);
    void Remove(GameObject* object);
    void Update(GameObject* object);

    void GetVisibleObjects(const std::function<bool(const AABB&)>& frustumCheck, std::vector<GameObject*>& results) const;

private:
    std::unique_ptr<OctreeNode> root_;
    AABB world_bounds_;
};

#endif //OGL_OCTREE_H
