#include "rigid_body.h"
#include "physics_manager.h"
#include "component/game_object.h"
#include "component/transform.h"
#include <glm/gtc/quaternion.hpp>

RigidBody::RigidBody() : rigidBody(nullptr), collisionShape(nullptr), motionState(nullptr) {
}

RigidBody::~RigidBody() {
    OnDestroy();
}

void RigidBody::Init(btCollisionShape* shape, float mass, const glm::vec3& localInertia) {
    collisionShape = shape;
    
    btVector3 btInertia(localInertia.x, localInertia.y, localInertia.z);
    if (mass != 0.0f) {
        collisionShape->calculateLocalInertia(mass, btInertia);
    }

    Transform* transform = game_object()->GetComponent<Transform>();
    glm::vec3 pos = transform->position();
    glm::vec3 rot = transform->rotation();
    
    // Euler to Quat (assuming degrees)
    glm::quat q = glm::quat(glm::radians(rot));
    
    btTransform btTrans;
    btTrans.setIdentity();
    btTrans.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));

    motionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, btInertia);
    rigidBody = new btRigidBody(rbInfo);

    PhysicsManager::GetInstance().GetWorld()->addRigidBody(rigidBody);
}

void RigidBody::Awake() {
}

void RigidBody::Update() {
    if (rigidBody && rigidBody->getMotionState()) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        
        Transform* transform = game_object()->GetComponent<Transform>();
        
        // Update position
        transform->set_position(glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
        
        // Update rotation
        btQuaternion btRot = trans.getRotation();
        glm::quat q(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());
        glm::vec3 euler = glm::degrees(glm::eulerAngles(q));
        transform->set_rotation(euler);
    }
}

void RigidBody::OnDestroy() {
    if (rigidBody) {
        if (PhysicsManager::GetInstance().GetWorld()) {
            PhysicsManager::GetInstance().GetWorld()->removeRigidBody(rigidBody);
        }
        delete rigidBody;
        rigidBody = nullptr;
    }
    if (motionState) {
        delete motionState;
        motionState = nullptr;
    }
    // Note: collisionShape might be shared, but in this simple implementation we delete it
    // In a real engine, we'd have a ShapeManager
    if (collisionShape) {
        delete collisionShape;
        collisionShape = nullptr;
    }
}

#include <rttr/registration>
RTTR_REGISTRATION {
    rttr::registration::class_<RigidBody>("RigidBody")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}
