#include "PhysicsEngineInterface.h"
#include "Math/Transform.h"
#include "Math/Matrix.h"
#include "Physics/BodyInstance.h"
#include "components/PrimitiveComponent.h"

FTransform FPhysicsEngineInterface::GetUnrealWorldTransform(const FBodyInstance* BodyInstance)
{
    FTransform OutTransform = FTransform::Identity;

    if (!BodyInstance || !BodyInstance->GetPhysicsActorHandle())
    {
        UE_LOG(ELogLevel::Error, TEXT("GetUnrealWorldTransform called on a BodyInstance with no ActorHandle!"));
        return OutTransform;
    }

    PxRigidActor* ActorHandle = BodyInstance->GetPhysicsActorHandle();
    PxTransform physxGlobalPose = ActorHandle->getGlobalPose();
    PxVec3 physxPos = physxGlobalPose.p;
    physx::PxQuat physxQuat = physxGlobalPose.q; // (x, y, z, w)

    FVector unrealPos(physxPos.x, physxPos.y, physxPos.z);
    FQuat unrealQuat = FQuat(physxQuat.x, physxQuat.y, physxQuat.z, physxQuat.w);
    FVector Scale = BodyInstance->OwnerComponent ? BodyInstance->OwnerComponent->GetComponentScale3D() : FVector(1.0f, 1.0f, 1.0f);
    OutTransform.SetTranslation(unrealPos);
    OutTransform.SetRotation(unrealQuat);
    OutTransform.SetScale3D(Scale); // PhysX는 스케일을 지원하지 않으므로 기본값 사용

    return OutTransform;
}

PxTransform FPhysicsEngineInterface::GetPhysXTransform(const FTransform& Transform)
{
    const FVector UnrealPos = Transform.GetTranslation();
    const FQuat UnrealQuat = Transform.GetRotation();

    const PxVec3 PhysXPos(UnrealPos.X, UnrealPos.Y, UnrealPos.Z); // PhysX 좌표계로 변환
    const PxQuat PhysXQuat(UnrealQuat.X, UnrealQuat.Y, UnrealQuat.Z, UnrealQuat.W);

    return PxTransform(PhysXPos, PhysXQuat); // PhysX 좌표계로 변환된 PxTransform 반환
}
