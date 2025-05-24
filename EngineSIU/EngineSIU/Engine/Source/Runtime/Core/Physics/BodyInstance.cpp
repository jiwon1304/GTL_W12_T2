#include "BodyInstance.h"
#include "PhysicsInterfaceTypesCore.h"
#include "PhysicsEngineInterface.h"
#include "Components/PrimitiveComponent.h"

FPhysicsActorHandle* FBodyInstance::GetPhysicsActorHandle()
{
    return ActorHandle;
}

FPhysicsActorHandle* FBodyInstance::GetPhysicsActorHandle() const
{
    return ActorHandle;
}

void FBodyInstance::SetBodyTransform(const FTransform& NewTransform)
{
    if (ActorHandle)
    {
        ActorHandle->setGlobalPose(FPhysicsEngineInterface::GetPhysXTransform(NewTransform));
    }
}

void FBodyInstance::UpdateBodyScale(const FVector& NewScale)
{
    // !TODO : Scale 업데이트 로직
    // Shape들에 대해서 Scale업데이트
}

bool FBodyInstance::IsInstanceSimulatingPhysics()
{
    return bSimulatePhysics;
}

FTransform FBodyInstance::GetUnrealWorldTransform()
{
    // !NOTE : 뭐가 잘 안되면 여기 보자
    // !TODO : 멀티스레드 환경이니 문제 없나 체크해야 함 -> Simulate한 후에 fetchResults하면서 대기하므로 그 외부에서 호출한다면 문제 없어보임

    if (!ActorHandle || !OwnerComponent)
    {
        UE_LOG(ELogLevel::Error, TEXT("GetUnrealWorldTransform called on a BodyInstance with no ActorHandle!"));
        return FTransform::Identity;
    }

    return FPhysicsEngineInterface::GetUnrealWorldTransform(this);
}
