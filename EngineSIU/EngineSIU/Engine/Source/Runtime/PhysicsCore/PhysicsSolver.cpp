#include "PhysicsSolver.h"
#include "PhysicsEngine/ShapeElem.h"
#include "PhysicsScene.h"
#include "PhysicsCore/PhysxSolversModule.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void FPhysicsSolver::Init()
{

}

void FPhysicsSolver::Release()
{

}


void FPhysicsSolver::InitScene(FPhysScene* InScene) const
{
    if (!InScene)
    {
        UE_LOG(ELogLevel::Error, "InScene is null!");
        return;
    }

    physx::PxScene* NewPxScene = FPhysxSolversModule::GetModule()->CreateScene();
    if (InScene == nullptr || NewPxScene == nullptr)
    {
        UE_LOG(ELogLevel::Error, "Failed to create FPhysScene or PxScene!");
        return;
    }
    InScene->Init(const_cast<FPhysicsSolver*>(this), NewPxScene);
}

// TODO : 일단 FBodyInstance를 사용해서 함
// 이후에는 UstaticMesh, USkeletalMesh,
// 추후에는 Actor단위로 받아서 할 것.
PxActor* FPhysicsSolver::RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance)
{
    if (!NewInstance)
    {
        UE_LOG(ELogLevel::Error, "NewInstance is null!");
        return nullptr;
    }

    PxScene* Scene = InScene->PhysxScene;
    if (!Scene)
    {
        UE_LOG(ELogLevel::Error, "PxScene is null!");
        return nullptr;
    }

    FMatrix InitialMatrix = NewInstance->OwnerComponent->GetWorldMatrix();
    FVector InitialPosition = InitialMatrix.GetTranslationVector();
    FQuat InitialRotation = InitialMatrix.ToQuat();
    
    PxVec3 Position(InitialPosition.X, InitialPosition.Y, InitialPosition.Z);
    PxQuat Rotation(InitialRotation.X, InitialRotation.Y, InitialRotation.Z, InitialRotation.W);

    PxTransform InitialTransform(Position, Rotation);

    PxPhysics* Physics = FPhysxSolversModule::GetModule()->Physics;

    const FKAggregateGeom& AggGeom = NewInstance->ExternalCollisionProfileBodySetup->AggGeom;
    float Volume = 0.f;
    PxRigidDynamic* NewDynamic = Physics->createRigidDynamic(InitialTransform);
    for (const FKBoxElem& BoxElem : AggGeom.BoxElems)
    {
        NewDynamic->userData = (void*)NewInstance; // 사용자 정의 데이터로 FBodyInstance를 설정
        PxBoxGeometry BoxGeometry(PxVec3(BoxElem.X, BoxElem.Y, BoxElem.Z)); // 크기
        PxShape* NewShape = Physics->createShape(BoxGeometry, *FPhysxSolversModule::GetModule()->DefaultMaterial); // 
        
        FVector Center = BoxElem.Center;
        FQuat Quat = BoxElem.Rotation.Quaternion();
        NewShape->setLocalPose(PxTransform(PxVec3(Center.X, Center.Y, Center.Z), PxQuat(Quat.X, Quat.Y, Quat.Z, Quat.W)));
        NewDynamic->attachShape(*NewShape);
        
        Volume = Volume + BoxElem.X * BoxElem.Y * BoxElem.Z;
    }
    float Mass = NewInstance->MassScale 
        * NewInstance->ExternalCollisionProfileBodySetup->PhysMaterial->Density * Volume;
    physx::PxRigidBodyExt::updateMassAndInertia(*NewDynamic, NewInstance->MassScale);
    Scene->addActor(*NewDynamic);

    return NewDynamic;
}

void FPhysicsSolver::AdvanceOneTimeStep(FPhysScene* InScene, float Dt)
{
    InScene->PhysxScene->simulate(Dt);
}

void FPhysicsSolver::FetchData(FPhysScene* InScene)
{
    InScene->PhysxScene->fetchResults(true);

    PxScene* Scene = InScene->PhysxScene;
    // 이렇게 해도 되고, 아니면 Actor에서 userdata를 가져와서 해도 되고...
    for (auto& Elem : InScene->RegisteredInstances)
    {
        FBodyInstance* BodyInstance = Elem.Key;
        PxActor* Actor = Elem.Value;

        switch (BodyInstance->ObjectType)
        {
        case ECollisionChannel::ECC_WorldDynamic:
        {
            PxRigidDynamic* DynamicActor = Actor->is<PxRigidDynamic>();
            if (DynamicActor)
            {
                PxTransform Transform = DynamicActor->getGlobalPose();
                BodyInstance->OwnerComponent->SetWorldTransform(
                    FTransform(
                        FQuat(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w),
                        FVector(Transform.p.x, Transform.p.y, Transform.p.z),
                        FVector(BodyInstance->Scale3D.X, BodyInstance->Scale3D.Y, BodyInstance->Scale3D.Z)
                    )
                );
            }
            break;
        }
        default:
            assert(0);
        }
    }
}

PxGeometryType::Enum FPhysicsSolver::GetPxType(const FKShapeElem* InShape)
{
    if (InShape->StaticStruct()->IsChildOf(FKBoxElem::StaticStruct()))
    {
        return PxGeometryType::eBOX;
    }
    else
    {
        assert(0);
    }
}
