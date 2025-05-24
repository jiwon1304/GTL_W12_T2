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
};
