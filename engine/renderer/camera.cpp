//
// Created by Nemo li on 2023/12/7.
//

#include "camera.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <rttr/registration>
#include "component/game_object.h"
#include "component/transform.h"

RTTR_REGISTRATION//注册反射
{
    rttr::registration::class_<Camera>("Camera")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

std::vector<Camera *> Camera::all_camera_;
Camera *Camera::current_camera_;

Camera::Camera()
        : Component(), clear_color_(49.f / 255, 77.f / 255, 121.f / 255, 1.f),
          clear_flag_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT), depth_(0),
          culling_mask_(0x01) {
    //默认获取现有Camera最大depth，设置当前Camera.depth +1
    if (all_camera_.size() > 0) {
        unsigned char max_depth = all_camera_.back()->depth();
        depth_ = max_depth + 1;
    }
    all_camera_.push_back(this);
}


Camera::~Camera() {
    auto iter = std::find(all_camera_.begin(), all_camera_.end(), this);
    if (iter != all_camera_.end()) {
        all_camera_.erase(iter);
    }
}

void Camera::SetView(const glm::vec3 &cameraForward, const glm::vec3 &cameraUp) {
    auto transform = game_object()->GetComponent<Transform>();
    view_mat4_ = glm::lookAt(transform->position(), cameraForward, cameraUp);
}

void Camera::SetPerspective(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
    projection_mat4_ = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float z_near, float z_far) {
    projection_mat4_ = glm::ortho(left, right, bottom, top, z_near, z_far);
}

void Camera::Clear() {
    glClear(clear_flag_);
    glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
}

void Camera::set_depth(unsigned char depth) {
    if (depth_ == depth) {
        return;
    }
    depth_ = depth;
    Sort();
}

void Camera::Sort() {
    std::sort(all_camera_.begin(), all_camera_.end(), [](Camera *a, Camera *b) {
        return a->depth() < b->depth();
    });
}

void Camera::Foreach(std::function<void()> func) {
    for (auto iter = all_camera_.begin(); iter != all_camera_.end(); iter++) {
        current_camera_ = *iter;
        current_camera_->Clear();
        func();
    }
}