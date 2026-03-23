#ifndef OGL_RIGID_BODY_H
#define OGL_RIGID_BODY_H

#include "component/component.h"
#include <btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>

class RigidBody : public Component {
public:
    RigidBody();
    virtual ~RigidBody();

    void Init(btCollisionShape* shape, float mass = 1.0f, const glm::vec3& localInertia = glm::vec3(0, 0, 0));

    void Awake() override;
    void Update() override;
    void OnDestroy() override;

    btRigidBody* GetBtRigidBody() { return rigidBody; }

private:
    btRigidBody* rigidBody;
    btCollisionShape* collisionShape;
    btDefaultMotionState* motionState;

RTTR_ENABLE(Component);
};

#endif //OGL_RIGID_BODY_H
