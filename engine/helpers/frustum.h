#ifndef OGL_FRUSTUM_H
#define OGL_FRUSTUM_H

#include <glm/glm.hpp>
#include "aabb.h"

class Frustum {
public:
    void Update(const glm::mat4& viewProjection) {
        // Extract planes from VP matrix
        // Left
        planes[0].x = viewProjection[0].w + viewProjection[0].x;
        planes[0].y = viewProjection[1].w + viewProjection[1].x;
        planes[0].z = viewProjection[2].w + viewProjection[2].x;
        planes[0].w = viewProjection[3].w + viewProjection[3].x;

        // Right
        planes[1].x = viewProjection[0].w - viewProjection[0].x;
        planes[1].y = viewProjection[1].w - viewProjection[1].x;
        planes[1].z = viewProjection[2].w - viewProjection[2].x;
        planes[1].w = viewProjection[3].w - viewProjection[3].x;

        // Top
        planes[2].x = viewProjection[0].w - viewProjection[0].y;
        planes[2].y = viewProjection[1].w - viewProjection[1].y;
        planes[2].z = viewProjection[2].w - viewProjection[2].y;
        planes[2].w = viewProjection[3].w - viewProjection[3].y;

        // Bottom
        planes[3].x = viewProjection[0].w + viewProjection[0].y;
        planes[3].y = viewProjection[1].w + viewProjection[1].y;
        planes[3].z = viewProjection[2].w + viewProjection[2].y;
        planes[3].w = viewProjection[3].w + viewProjection[3].y;

        // Near
        planes[4].x = viewProjection[0].w + viewProjection[0].z;
        planes[4].y = viewProjection[1].w + viewProjection[1].z;
        planes[4].z = viewProjection[2].w + viewProjection[2].z;
        planes[4].w = viewProjection[3].w + viewProjection[3].z;

        // Far
        planes[5].x = viewProjection[0].w - viewProjection[0].z;
        planes[5].y = viewProjection[1].w - viewProjection[1].z;
        planes[5].z = viewProjection[2].w - viewProjection[2].z;
        planes[5].w = viewProjection[3].w - viewProjection[3].z;

        // Normalize planes
        for (int i = 0; i < 6; ++i) {
            float length = glm::length(glm::vec3(planes[i]));
            planes[i] /= length;
        }
    }

    bool IsBoxVisible(const AABB& aabb) const {
        for (int i = 0; i < 6; ++i) {
            glm::vec3 p = aabb.min;
            if (planes[i].x >= 0) p.x = aabb.max.x;
            if (planes[i].y >= 0) p.y = aabb.max.y;
            if (planes[i].z >= 0) p.z = aabb.max.z;

            if (glm::dot(glm::vec3(planes[i]), p) + planes[i].w < 0) {
                return false;
            }
        }
        return true;
    }

private:
    glm::vec4 planes[6];
};

#endif //OGL_FRUSTUM_H
