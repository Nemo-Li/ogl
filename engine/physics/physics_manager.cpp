#include "physics_manager.h"
#include "helpers/debug.h"

void PhysicsManager::Init() {
    DEBUG_LOG_INFO("PhysicsManager::Init");
    
    // Collision configuration contains default setup for memory, collision setup.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    // Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher.
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // btDbvtBroadphase is a good general purpose broadphase.
    broadphase = new btDbvtBroadphase();

    // The default constraint solver. For parallel processing you can use a different solver.
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // Default gravity
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

void PhysicsManager::Update(float deltaTime) {
    if (dynamicsWorld) {
        dynamicsWorld->stepSimulation(deltaTime, 10);
    }
}

void PhysicsManager::Cleanup() {
    if (dynamicsWorld) {
        // Remove the rigidbodies from the dynamics world and delete them
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        delete dynamicsWorld;
        delete solver;
        delete broadphase;
        delete dispatcher;
        delete collisionConfiguration;

        dynamicsWorld = nullptr;
        solver = nullptr;
        broadphase = nullptr;
        dispatcher = nullptr;
        collisionConfiguration = nullptr;
    }
}
