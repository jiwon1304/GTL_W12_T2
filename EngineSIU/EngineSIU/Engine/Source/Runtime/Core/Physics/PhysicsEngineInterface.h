#pragma once
#include "PhysicsInterfaceTypesCore.h"

struct FTransform;
struct FBodyInstance;

using namespace physx;
class FPhysicsEngineInterface
{
public:
    static bool IsValid(const PxRigidActor* Handle) { return Handle != nullptr; }
    static FTransform GetUnrealWorldTransform(const FBodyInstance* Handle);
    static PxTransform GetPhysXTransform(const FTransform& Transform);
};
