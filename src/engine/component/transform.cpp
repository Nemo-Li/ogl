//
// Created by Nemo li on 2023/12/6.
//

#include "transform.h"
#include <rttr/registration>

//注册反射
RTTR_REGISTRATION {
    rttr::registration::class_<Transform>("Transform")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr)
            .property("position", &Transform::position, &Transform::set_position)
            .property("rotation", &Transform::rotation, &Transform::set_rotation)
            .property("scale", &Transform::scale, &Transform::set_scale);
}

Transform::Transform() : position_(0.f), rotation_(0.f), scale_(1.f) {
}

Transform::~Transform() {
}