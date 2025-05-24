#include "PhysXManager.h"
#include <UserInterface/Console.h>
#include "World/World.h"
#include "Physics/PhysScene.h"
#include <memory>

bool FPhysXManager::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    if (!gFoundation)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxCreateFoundation failed"));
        return false; // Foundation 생성 실패
    }

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxCreatePhysics failed"));
        return false; // Physics 생성 실패
    }

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);

    if (!gDispatcher)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxDefaultCpuDispatcherCreate failed"));
        return false; // Dispatcher 생성 실패
    }

    return true;
}

void FPhysXManager::Cleanup()
{
    if (gDispatcher)
    {
        gDispatcher->release();
        gDispatcher = nullptr;
    }

    if (gMaterial)
    {
        gMaterial->release();
        gMaterial = nullptr;
    }

    if (gPhysics)
    {
        gPhysics->release();
        gPhysics = nullptr;
    }

    if (gFoundation)
    {
        gFoundation->release();
        gFoundation = nullptr;
    }
}

FPhysScene* FPhysXManager::CreatePhysScene(UWorld* InWorld)
{
    if (!InWorld)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreatePhysScene failed: InWorld is null"));
        return nullptr; // 월드가 유효하지 않음
    }

    PxScene* NewScene = gPhysics->createScene(GetDefaultSceneDesc());

    if (!NewScene)
    {
        UE_LOG(ELogLevel::Error, TEXT("Failed to create PhysX scene"));
        return nullptr; // 씬 생성 실패
    }
    
    NewScene->userData = InWorld;

    
    return new FPhysScene(NewScene);
}

FPhysicsActorHandle* FPhysXManager::CreateRigidDynamic(const PxTransform& InTransform)
{
    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreateRigidDynamic failed: gPhysics is null"));
        return nullptr; // Physics가 초기화되지 않음
    }
    PxRigidDynamic* rigidDynamic = gPhysics->createRigidDynamic(InTransform);

    return rigidDynamic;
}

PxSceneDesc FPhysXManager::GetDefaultSceneDesc()
{
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0, -9.81f, 0);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;

    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    return sceneDesc;
}

FPhysXManager::~FPhysXManager()
{
    Cleanup();
}
