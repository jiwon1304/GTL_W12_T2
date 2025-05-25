#include "PhysXIntegration.h"

#include "Engine/Source/Runtime/Physics/PhysicsEngine/BodyInstance.h"

PxDefaultAllocator      gAllocator;
PxDefaultErrorCallback  gErrorCallback;
PxFoundation* gFoundation = nullptr;
PxPhysics* gPhysics = nullptr;
PxScene* gScene = nullptr;
PxMaterial* gMaterial = nullptr;
PxDefaultCpuDispatcher* gDispatcher = nullptr;

std::vector<GameObject> gObjects;

void InitPhysX() {
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0, 0, -9.81f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);
}

void ReleasePhysX()
{
    // 1) Scene 해제
    if (gScene)
    {
        gScene->release();
        gScene = nullptr;
    }

    // 2) CpuDispatcher 해제
    if (gDispatcher)
    {
        gDispatcher->release();
        gDispatcher = nullptr;
    }

    // 3) Material 해제
    if (gMaterial)
    {
        gMaterial->release();
        gMaterial = nullptr;
    }

    // 4) Physics 해제
    if (gPhysics)
    {
        gPhysics->release();
        gPhysics = nullptr;
    }

    // 5) Foundation 해제
    if (gFoundation)
    {
        gFoundation->release();
        gFoundation = nullptr;
    }

    // 6) gObjects 해제
    gObjects.clear();
}

GameObject CreateBox(const PxVec3& pos, const PxVec3& halfExtents) {
    GameObject obj;
    PxTransform pose(pos);
    obj.rigidBody = gPhysics->createRigidDynamic(pose);
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial);
    obj.rigidBody->attachShape(*shape);
    //PxRigidBodyExt::updateMassAndInertia(*obj.rigidBody, 10.0f);
    gScene->addActor(*obj.rigidBody);
    obj.UpdateFromPhysics();
    return obj;
}

GameObject CreateGameObject(EPhysicsType PhysicsType, const PxVec3& pos, const float density)
{
    GameObject obj;
    PxTransform pose(pos);
    switch (PhysicsType) 
    {
        case EPhysicsType::PhysType_Default:
        {
            // Static: PxRigidStatic
            PxRigidStatic* body = gPhysics->createRigidStatic(pose);
            obj.rigidBody = body;
        }
        break;

        case EPhysicsType::PhysType_Kinematic:
        {
            // Kinematic
            PxRigidDynamic* body = gPhysics->createRigidDynamic(pose);
            body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
            obj.rigidBody = body;
        }
        break;

        case EPhysicsType::PhysType_Simulated:
        {
            // Simulated
            PxRigidDynamic* body = gPhysics->createRigidDynamic(pose);
            
            // Compute mass & inertia for simulation
            PxRigidBodyExt::updateMassAndInertia(*body, density);
            obj.rigidBody = body;
        }
        break;

        default:
        {
            // Fallback to static
            PxRigidStatic* body = gPhysics->createRigidStatic(pose);
            obj.rigidBody = body;
        }
        break;    
    }

    return obj;
}

void Simulate(float dt) {
    gScene->simulate(dt);
    gScene->fetchResults(true);
    for (auto& obj : gObjects) obj.UpdateFromPhysics();
}

void GameObject::UpdateFromPhysics() {
    PxTransform t = rigidBody->getGlobalPose();
    PxMat44 mat(t);

    BodyInstance->UpdateFromPhysics(mat);
}
