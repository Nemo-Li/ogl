#include "octree.h"
#include "component/game_object.h"
#include "component/transform.h"

OctreeNode::OctreeNode(const AABB& bounds, int depth) : bounds_(bounds), depth_(depth) {
    for (int i = 0; i < 8; ++i) {
        children_[i] = nullptr;
    }
}

OctreeNode::~OctreeNode() {}

void OctreeNode::Subdivide() {
    glm::vec3 center = bounds_.Center();
    glm::vec3 halfSize = bounds_.Size() * 0.5f;

    for (int i = 0; i < 8; ++i) {
        glm::vec3 min, max;
        min.x = (i & 1) ? center.x : bounds_.min.x;
        min.y = (i & 2) ? center.y : bounds_.min.y;
        min.z = (i & 4) ? center.z : bounds_.min.z;
        max = min + halfSize;
        children_[i] = std::make_unique<OctreeNode>(AABB(min, max), depth_ + 1);
    }

    // Move objects from this node to children if they fit
    for (auto it = objects_.begin(); it != objects_.end(); ) {
        bool moved = false;
        for (int i = 0; i < 8; ++i) {
            if (children_[i]->bounds_.Contains(it->second.min) && children_[i]->bounds_.Contains(it->second.max)) {
                children_[i]->Insert(it->first, it->second);
                it = objects_.erase(it);
                moved = true;
                break;
            }
        }
        if (!moved) {
            ++it;
        }
    }
}

void OctreeNode::Insert(GameObject* object, const AABB& objectBounds) {
    if (IsLeaf()) {
        if (objects_.size() < MAX_OBJECTS_PER_NODE || depth_ >= MAX_DEPTH) {
            objects_.push_back({object, objectBounds});
            return;
        }
        Subdivide();
    }

    for (int i = 0; i < 8; ++i) {
        if (children_[i]->bounds_.Contains(objectBounds.min) && children_[i]->bounds_.Contains(objectBounds.max)) {
            children_[i]->Insert(object, objectBounds);
            return;
        }
    }
    
    // If it doesn't fit in any child, store it in this node
    objects_.push_back({object, objectBounds});
}

void OctreeNode::Remove(GameObject* object) {
    for (auto it = objects_.begin(); it != objects_.end(); ++it) {
        if (it->first == object) {
            objects_.erase(it);
            return;
        }
    }

    if (!IsLeaf()) {
        for (int i = 0; i < 8; ++i) {
            children_[i]->Remove(object);
        }
    }
}

void OctreeNode::GetObjectsInFrustum(const std::function<bool(const AABB&)>& frustumCheck, std::vector<GameObject*>& results) const {
    if (!frustumCheck(bounds_)) {
        return;
    }

    for (const auto& pair : objects_) {
        if (frustumCheck(pair.second)) {
            results.push_back(pair.first);
        }
    }

    if (!IsLeaf()) {
        for (int i = 0; i < 8; ++i) {
            children_[i]->GetObjectsInFrustum(frustumCheck, results);
        }
    }
}

Octree::Octree(const AABB& worldBounds) : world_bounds_(worldBounds) {
    root_ = std::make_unique<OctreeNode>(world_bounds_);
}

void Octree::Insert(GameObject* object) {
    root_->Insert(object, object->aabb());
}

void Octree::Remove(GameObject* object) {
    root_->Remove(object);
}

void Octree::Update(GameObject* object) {
    root_->Remove(object);
    root_->Insert(object, object->aabb());
}

void Octree::GetVisibleObjects(const std::function<bool(const AABB&)>& frustumCheck, std::vector<GameObject*>& results) const {
    root_->GetObjectsInFrustum(frustumCheck, results);
}
