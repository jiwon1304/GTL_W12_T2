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

    PhysicsObjects.Empty();
    OwningWorld = nullptr;
}

void FPhysScene::Tick(float DeltaTime)
{
    Simulate(DeltaTime);
}

void FPhysScene::AddPhysicsObject(class UPrimitiveComponent* Component)
{
    assert(Scene);
    if (!Component)
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::AddPhysicsObject called with null Component!"));
        return;
    }

    FBodyInstance* Body = Component->GetBodyInstance();
    Body->OwnerComponent = Component;

    if(Body->bSimulatePhysics)
        PhysicsObjects.Add(Body);

    //FMatrix ComponentWorldMatrix = Component->GetWorldMatrix();
    //FTransform ComponentWorldTransform = FTransform(ComponentWorldMatrix);
    //Body->ActorHandle = FPhysXManager::Get().CreateRigidDynamic(FPhysicsEngineInterface::GetPhysXTransform(ComponentWorldTransform));
}

void FPhysScene::RemovePhysicsObject(UPrimitiveComponent* Component)
{
    if (!Component)
        return;

    FBodyInstance* Body = Component->GetBodyInstance();
    if (!Body || !Body->IsValid())
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::RemovePhysicsObject called with invalid BodyInstance!"));
        return;
    }

    PhysicsObjects.Remove(Body);
}

void FPhysScene::Simulate(float DeltaTime)
{
    if (!bIsSimulating || !Scene)
    {
        //UE_LOG(ELogLevel::Error, TEXT("FPhysScene::Simulate called on a scene that is not initialized!"));
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

void FPhysScene::StartSimulation()
{
    if (!Scene || !OwningWorld)
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::StartSimulation called on a scene that is not initialized!"));
        return;
    }
    bIsSimulating = true;
    // !TODO : Scene->setSimulationEventCallback(FPhysicsEngineInterface::GetPhysXSimulationEventCallback());

    // !TODO : World의 레벨에서 Component들 순회하면서 PhysicsObjects에 추가
    for (auto& Actor : OwningWorld->GetActiveLevel()->Actors)
    {
        if (Actor)
        {
            for (UActorComponent* Component : Actor->GetComponents())
            {
                if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
                {
                    AddPhysicsObject(PrimitiveComponent);
                }
            }
        }
    }

    for (FBodyInstance* Body : PhysicsObjects)
    {
        if (Body && Body->OwnerComponent)
        {
            FMatrix ComponentWorldMatrix = Body->OwnerComponent->GetWorldMatrix();
            FTransform ComponentWorldTransform = FTransform(ComponentWorldMatrix);
            
            Body->SetPhysicsActorHandle(FPhysXManager::Get().CreateRigidDynamic(FPhysicsEngineInterface::GetPhysXTransform(ComponentWorldTransform)));
            Scene->addActor(*Body->ActorHandle);
            Body->OwnerComponent->ApplyBodySetup(Body);
        }
    }
}

void FPhysScene::StopSimulation()
{
    if (!Scene)
    {
        UE_LOG(ELogLevel::Error, TEXT("FPhysScene::StopSimulation called on a scene that is not initialized!"));
        return;
    }
    bIsSimulating = false;
    for (FBodyInstance* Body : PhysicsObjects)
    {
        if (Body && Body->IsValid())
        {
            Scene->removeActor(*Body->ActorHandle);
        }
    }
}
