#pragma once


class UPrimitiveComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;

namespace physx 
{
    class PxMat44;
}

struct FBodyInstance 
{
    UPrimitiveComponent* OwnerComponent;

    bool bSimulatePhysics = false;

    void CreatePhysXActor();

    void CreatePhysXActor(UStaticMeshComponent* OwnerComponent);

    void CreatePhysXActor(USkeletalMeshComponent* OwnerComponent);

    void UpdateFromPhysics(const physx::PxMat44& InMatrix);
};
