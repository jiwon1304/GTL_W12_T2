#include "PhysScene.h"
#include "World/World.h"
#include "Components/PrimitiveComponent.h"
#include "Physics/PhysXManager.h"
#include "UserInterface/Console.h"

FPhysScene::FPhysScene(physx::PxScene* InScene)
{
    OwningWorld = reinterpret_cast<UWorld*>(InScene->userData);
    if (!OwningWorld)
    {
        UE_LOG(ELogLevel::Error, TEXT("PxScene userData is not set to UWorld!"));
        delete this;
        return;
    }

    if (!InScene)
    {
        delete this;
        return;
    }
    Scene = InScene;
}

FPhysScene::~FPhysScene()
{
    if (Scene)
    {
        Scene->release();
        Scene = nullptr;
    }
    for (FBodyInstance* Body : PhysicsObjects)
    {
        if (Body)
        {
            delete Body;
        }
    }
    PhysicsObjects.Empty();
    OwningWorld = nullptr;
}

void FPhysScene::Tick(float DeltaTime)
{
}

FBodyInstance* FPhysScene::AddPhysicsObject(UPrimitiveComponent* Component)
{
    assert(Scene);
    if (!Component)
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::AddPhysicsObject called with null Component!"));
        return nullptr;
    }

    FBodyInstance* NewBody = new FBodyInstance();
    NewBody->OwnerComponent = Component;
    FMatrix ComponentWorldMatrix = Component->GetWorldMatrix();
    FTransform ComponentWorldTransform = FTransform(ComponentWorldMatrix);
    NewBody->ActorHandle = FPhysXManager::Get().CreateRigidDynamic(FPhysicsEngineInterface::GetPhysXTransform(ComponentWorldTransform));
}

void FPhysScene::RemovePhysicsObject(FBodyInstance* BodyToRemove)
{
}

void FPhysScene::Simulate(float DeltaTime)
{
    if (!Scene)
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::Simulate called on a scene that is not initialized!"));
        return;
    }
    Scene->simulate(DeltaTime);
    Scene->fetchResults(true);
    for (FBodyInstance* Body : PhysicsObjects)
    {
        if (Body && Body->IsValid())
        {
            Body->OwnerComponent->SyncComponentToRBPhysics();
        }
    }
}
