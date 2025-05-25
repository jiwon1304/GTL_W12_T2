#include "PhysicsScene.h"
#include "Components/ShapeComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "PhysicsCore/PhysXIntegration.h"
#include "PhysicsSolver.h"
#include "PhysxSolversModule.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"



void FPhysScene::Init(FPhysicsSolver* InSceneSolver, physx::PxScene* InScene)
{
    if (!InSceneSolver)
    {
        UE_LOG(ELogLevel::Error, "SceneSolver is null!");
        return;
    }

    SceneSolver = InSceneSolver;

    PhysxScene = InScene;
}

void FPhysScene::AddActor(AActor* Actor)
{
    TSet<UActorComponent*> ActorComponents = Actor->GetComponents();

    // StaticMeshComponent
    {
        TSet<UStaticMeshComponent*> StaticMeshComponents;
        for (UActorComponent* Component : ActorComponents)
        {
            if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
            {
                StaticMeshComponents.Add(StaticMeshComponent);
            }
        }

        for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
        {
            UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
            if (!StaticMesh)
            {
                UE_LOG(ELogLevel::Warning, TEXT("StaticMeshComponent '%s' has no StaticMesh assigned."), *StaticMeshComponent->GetName());
                continue;
            }
            
            FBodyInstance* BodyInstance = &StaticMesh->GetBodySetup()->DefaultInstance;
            PxActor* RegisteredActor = SceneSolver->RegisterObject(this, BodyInstance);

            RegisteredInstances.Add(BodyInstance, RegisteredActor);
        }
    }

    // TODO : SkeletalMeshComponent
}

void FPhysScene::AdvanceAndDispatch_External(float DeltaTime)
{
    SceneSolver->AdvanceOneTimeStep(this, DeltaTime);
}
