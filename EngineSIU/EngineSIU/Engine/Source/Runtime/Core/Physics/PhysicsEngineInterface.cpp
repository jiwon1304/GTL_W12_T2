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

    FPhysicsActorHandle* ActorHandle = BodyInstance->GetPhysicsActorHandle();
    PxTransform physxGlobalPose = ActorHandle->getGlobalPose();
    PxVec3 physxPos = physxGlobalPose.p;
    physx::PxQuat physxQuat = physxGlobalPose.q; // (x, y, z, w)

    FVector unrealPos(physxPos.z, physxPos.x, physxPos.y);

    // 한 번만 계산되도록 static const로 선언합니다.
    static const FQuat PhysXToUnrealRotationConv = []() {
        // PhysX 좌표계의 축들을 Unreal 좌표계에서 어떻게 표현되는지로 정의합니다.
        // PhysX X-axis (Right) in Unreal is Unreal Y-axis (0,1,0)
        // PhysX Y-axis (Up)    in Unreal is Unreal Z-axis (0,0,1)
        // PhysX Z-axis (Fwd)   in Unreal is Unreal X-axis (1,0,0)
        FMatrix basisMatrix;
        basisMatrix.SetAxis(0, FVector(0.f, 1.f, 0.f)); // PhysX X-axis in Unreal coords
        basisMatrix.SetAxis(1, FVector(0.f, 0.f, 1.f)); // PhysX Y-axis in Unreal coords
        basisMatrix.SetAxis(2, FVector(1.f, 0.f, 0.f)); // PhysX Z-axis in Unreal coords
        basisMatrix.SetAxis(3, FVector::ZeroVector);    // Translation part (not used for rotation)
        return basisMatrix.ToQuat();
        }();

    FQuat physxQuatAsFQuat(physxQuat.x, physxQuat.y, physxQuat.z, physxQuat.w);
    FQuat unrealQuat = PhysXToUnrealRotationConv * physxQuatAsFQuat;

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

    const PxVec3 PhysXPos(UnrealPos.Y, UnrealPos.Z, UnrealPos.X); // PhysX 좌표계로 변환

    static const FQuat PhysXUnrealRotatorConv = []()
    {
        FMatrix BasisMatrix;
        BasisMatrix.SetAxis(0, FVector(0.f, 1.f, 0.f)); // PhysX X-axis (Right) is Unreal Y-axis (Right)
        BasisMatrix.SetAxis(1, FVector(0.f, 0.f, 1.f)); // PhysX Y-axis (Up)    is Unreal Z-axis (Up)
        BasisMatrix.SetAxis(2, FVector(1.f, 0.f, 0.f)); // PhysX Z-axis (Fwd)   is Unreal X-axis (Fwd)
        // BasisMatrix.SetAxis(3, FVector::ZeroVector); // W component is not used for FQuat from FMatrix
        return BasisMatrix.ToQuat();
    }();

    static const FQuat UnrealToPhysXRotationConv = UnrealToPhysXRotationConv.Inverse();

    const FQuat PhysXQuatAsFQuat = UnrealToPhysXRotationConv * UnrealQuat; // Unreal -> PhysX 변환
    const PxQuat PhysXQuat(PhysXQuatAsFQuat.X, PhysXQuatAsFQuat.Y, PhysXQuatAsFQuat.Z, PhysXQuatAsFQuat.W);

    return PxTransform(PhysXPos, PhysXQuat); // PhysX 좌표계로 변환된 PxTransform 반환
}
