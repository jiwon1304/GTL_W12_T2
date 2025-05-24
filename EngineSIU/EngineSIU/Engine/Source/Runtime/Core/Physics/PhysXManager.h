#pragma once
#include <PxPhysicsAPI.h>
#include "Math/Vector.h"

class FPhysXManager
{
public:
    static FPhysXManager& Get()
    {
        static FPhysXManager Instance;
        return Instance;
    };

    FPhysXManager(const FPhysXManager&) = delete;
    FPhysXManager& operator=(const FPhysXManager&) = delete;

    bool Init();
    void Cleanup();



private:
    FPhysXManager() = default;
    ~FPhysXManager() = default;
};
