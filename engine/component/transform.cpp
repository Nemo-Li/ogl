#include "transform.h"
#include <rttr/registration>
#include "game_object.h"

//注册反射
RTTR_REGISTRATION {
    rttr::registration::class_<Transform>("Transform")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr)
            .property("position", &Transform::position, &Transform::set_position)
            .property("rotation", &Transform::rotation, &Transform::set_rotation)
            .property("scale", &Transform::scale, &Transform::set_scale);
}

Transform::Transform() : Component(), position_(0.f), rotation_(0.f), scale_(1.f) {
}

Transform::~Transform() {
}

void Transform::set_position(glm::vec3 position) {
    position_ = position;
    if (game_object()) {
        game_object()->UpdateAABB();
    }
}

void Transform::set_rotation(glm::vec3 rotation) {
    rotation_ = rotation;
    if (game_object()) {
        game_object()->UpdateAABB();
    }
}

void Transform::set_scale(glm::vec3 scale) {
    scale_ = scale;
    if (game_object()) {
        game_object()->UpdateAABB();
    }
}
