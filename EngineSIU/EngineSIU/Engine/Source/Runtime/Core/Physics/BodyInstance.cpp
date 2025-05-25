#include "BodyInstance.h"
#include "PhysicsInterfaceTypesCore.h"
#include "PhysicsEngineInterface.h"
#include "Components/PrimitiveComponent.h"
#include "Physics/PhysXManager.h"

PxRigidActor* FBodyInstance::GetPhysicsActorHandle()
{
    return ActorHandle;
}

PxRigidActor* FBodyInstance::GetPhysicsActorHandle() const
{
    return ActorHandle;
}

void FBodyInstance::SetPhysicsActorHandle(PxRigidActor* InActorHandle)
{
    if (ActorHandle)
    {
        UE_LOG(ELogLevel::Warning, TEXT("Setting ActorHandle on FBodyInstance that already has one! Previous handle will be replaced."));
        ActorHandle->release(); // 기존 핸들을 해제
    }
    ActorHandle = InActorHandle;
}

void FBodyInstance::SetBodyTransform(const FTransform& NewTransform)
{
    if (ActorHandle)
    {
        ActorHandle->setGlobalPose(FPhysicsEngineInterface::GetPhysXTransform(NewTransform));
        // !TODO : 스케일
    }
}

void FBodyInstance::UpdateBodyScale(const FVector& NewScale)
{
    // !TODO : Scale 업데이트 로직
    // Shape들에 대해서 Scale업데이트
}

void FBodyInstance::AttachBox(const FKBoxElem* BoxElem)
{
    if (!ActorHandle)
    {
        UE_LOG(ELogLevel::Error, TEXT("AttachBox called on a BodyInstance with no ActorHandle!"));
        return;
    }
    PxShape* shape = FPhysXManager::Get().CreateBoxShape(*BoxElem);
    if (shape)
        ActorHandle->attachShape(*shape);
}

void FBodyInstance::AttachSphere(const FKSphereElem* SphereElem)
{
    if (!ActorHandle)
    {
        UE_LOG(ELogLevel::Error, TEXT("AttachSphere called on a BodyInstance with no ActorHandle!"));
        return;
    }
    PxShape* shape = FPhysXManager::Get().CreateSphereShape(*SphereElem);
    if (shape)
        ActorHandle->attachShape(*shape);
}

void FBodyInstance::AttachSphyl(const FKSphylElem* SphylElem)
{
    if (!ActorHandle)
    {
        UE_LOG(ELogLevel::Error, TEXT("AttachSphyl called on a BodyInstance with no ActorHandle!"));
        return;
    }
    PxShape* shape = FPhysXManager::Get().CreateSphylShape(*SphylElem);
    if (shape)
        ActorHandle->attachShape(*shape);
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
