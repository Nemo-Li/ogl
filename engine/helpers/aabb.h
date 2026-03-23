#ifndef OGL_AABB_H
#define OGL_AABB_H

#include <glm/glm.hpp>
#include <vector>

class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
    AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}

    glm::vec3 Center() const { return (min + max) * 0.5f; }
    glm::vec3 Size() const { return max - min; }

    bool Contains(const glm::vec3& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }

    bool Intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    void Encapsulate(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    void Encapsulate(const AABB& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }
};

#endif //OGL_AABB_H
