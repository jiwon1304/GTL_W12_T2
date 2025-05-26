#pragma once
#include "UObject/ObjectMacros.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Math/Transform.h"
#include "Physics/PhysicsInterfaceTypesCore.h"

class UPrimitiveComponent;
struct FKShapeElem;

// 게임 스레드의 컴포넌트 트랜스폼과 물리 엔진 스레드의 중간다리 역할 클래스
struct FBodyInstance
{
    DECLARE_STRUCT(FBodyInstance)
    class UPrimitiveComponent* OwnerComponent;

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool, bSimulatePhysics, = false
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool, bIsKinematic, = false
    )

    PxRigidActor* ActorHandle = nullptr;
    PxRigidActor* GetPhysicsActorHandle();
    PxRigidActor* GetPhysicsActorHandle() const;

    void SetPhysicsActorHandle(PxRigidActor* InActorHandle);

    bool IsValid() const
    {
        return ActorHandle != nullptr && OwnerComponent != nullptr;
    }

    void SetBodyTransform(const FTransform& NewTransform);
    void UpdateBodyScale(const FVector& NewScale);
    void AttachBox(const struct FKBoxElem* BoxElem);
    void AttachSphere(const struct FKSphereElem* SphereElem);
    void AttachSphyl(const struct FKSphylElem* SphylElem);
    bool IsInstanceSimulatingPhysics();
    FTransform GetUnrealWorldTransform();
};
