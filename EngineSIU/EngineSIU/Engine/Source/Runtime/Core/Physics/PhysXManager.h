#pragma once
#include <PxPhysicsAPI.h>
#include "Math/Vector.h"
#include "PhysicsEngineInterface.h"
#include "PhysicsInterfaceTypesCore.h"

struct FPhysScene;

class FPhysXManager
{
public:
    static FPhysXManager& Get()
    {
        static FPhysXManager Instance;
        return Instance;
    };

    bool Init();
    void Cleanup();

public:
    FPhysScene* CreatePhysScene(class UWorld* InWorld);
    void ReleasePhysScene(FPhysScene* InPhysScene);
    PxRigidDynamic* CreateRigidDynamic(const PxTransform& InTransform);
    PxShape* CreateBoxShape(const struct FKBoxElem& BoxElem);
    PxShape* CreateSphereShape(const struct FKSphereElem& SphereElem);
    PxShape* CreateSphylShape(const struct FKSphylElem& SphylElem);
private:
    PxSceneDesc GetDefaultSceneDesc();

private:
    FPhysXManager() = default;
    ~FPhysXManager();

private:
    PxFoundation*               gFoundation;
    PxPhysics*                  gPhysics;
    PxDefaultAllocator          gAllocator;
    PxDefaultErrorCallback      gErrorCallback;
    PxDefaultCpuDispatcher*     gDispatcher;
    PxMaterial*                 gMaterial;              // 피직스 머티리얼은 일단 싱글톤에서 하나로 관리함
    PxPvd*                      gPvd;                   // 디버깅을 위한 pvd
    PxPvdTransport*             gTransport;
};
