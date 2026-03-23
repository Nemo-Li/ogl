#ifndef OGL_PHYSICS_MANAGER_H
#define OGL_PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>
#include <vector>

class PhysicsManager {
public:
    static PhysicsManager& GetInstance() {
        static PhysicsManager instance;
        return instance;
    }

    void Init();
    void Update(float deltaTime);
    void Cleanup();

    btDiscreteDynamicsWorld* GetWorld() { return dynamicsWorld; }

private:
    PhysicsManager() : 
        broadphase(nullptr), 
        collisionConfiguration(nullptr), 
        dispatcher(nullptr), 
        solver(nullptr), 
        dynamicsWorld(nullptr) {}
    
    ~PhysicsManager() { Cleanup(); }

    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
};

#endif //OGL_PHYSICS_MANAGER_H
