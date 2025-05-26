#include "PhysXManager.h"
#include <UserInterface/Console.h>
#include "World/World.h"
#include "Physics/PhysScene.h"
#include <memory>
#include "Physics/BoxElem.h"
#include "Physics/SphereElem.h"
#include "Physics/SphylElem.h"

#pragma comment(lib, "PhysXPvdSDK_static_64.lib")


bool FPhysXManager::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    if (!gFoundation)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxCreateFoundation failed"));
        return false; // Foundation 생성 실패
    }

    gPvd = PxCreatePvd(*gFoundation);
    gTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
#if _DEBUG
    gPvd->connect(*gTransport, PxPvdInstrumentationFlag::eALL);
#else
    gPvd->connect(*gTransport, PxPvdInstrumentationFlag::eNone);
#endif

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxCreatePhysics failed"));
        return false; // Physics 생성 실패
    }

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    gDispatcher = PxDefaultCpuDispatcherCreate(4);

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

void FPhysXManager::ReleasePhysScene(FPhysScene* InPhysScene)
{
    if (!InPhysScene)
    {
        UE_LOG(ELogLevel::Error, TEXT("ReleasePhysScene failed: InPhysScene is null"));
        return; // PhysScene이 유효하지 않음
    }
    delete InPhysScene; // FPhysScene 객체 삭제
}

physx::PxRigidDynamic* FPhysXManager::CreateRigidDynamic(const PxTransform& InTransform, bool bIsKinematic)
{
    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreateRigidDynamic failed: gPhysics is null"));
        return nullptr; // Physics가 초기화되지 않음
    }
    PxRigidDynamic* rigidDynamic = gPhysics->createRigidDynamic(InTransform);
    PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, 10.0f);

    rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, bIsKinematic);

    return rigidDynamic;
}

PxShape* FPhysXManager::CreateBoxShape(const FKBoxElem& BoxElem)
{
    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreateBoxShape failed: gPhysics is null"));
        return nullptr; // Physics가 초기화되지 않음
    }

    PxVec3 halfExtents(BoxElem.X * 0.5f, BoxElem.Y * 0.5f, BoxElem.Z * 0.5f);
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial);
    PxTransform transform = FPhysicsEngineInterface::GetPhysXTransform(BoxElem.GetTransform());
    shape->setLocalPose(transform);

    return shape;
}

PxShape* FPhysXManager::CreateSphereShape(const FKSphereElem& SphereElem)
{
    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreateSphereShape failed: gPhysics is null"));
        return nullptr; // Physics가 초기화되지 않음
    }
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(SphereElem.Radius), *gMaterial);
    PxTransform transform = FPhysicsEngineInterface::GetPhysXTransform(SphereElem.GetTransform());

    return shape;
}

PxShape* FPhysXManager::CreateSphylShape(const FKSphylElem& SphylElem)
{
    if (!gPhysics)
    {
        UE_LOG(ELogLevel::Error, TEXT("CreateSphylShape failed: gPhysics is null"));
        return nullptr; // Physics가 초기화되지 않음
    }

    PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(SphylElem.Radius, SphylElem.Length * 0.5f), *gMaterial);
    PxTransform transform = FPhysicsEngineInterface::GetPhysXTransform(SphylElem.GetTransform());

    return shape;
}

PxSceneDesc FPhysXManager::GetDefaultSceneDesc()
{
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0, 0, -98.1f);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
#if _DEBUG
    sceneDesc.flags |= PxSceneFlag::eENABLE_ENHANCED_DETERMINISM;
#endif // _DEBUG


    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    return sceneDesc;
}

FPhysXManager::~FPhysXManager()
{
    Cleanup();
}
