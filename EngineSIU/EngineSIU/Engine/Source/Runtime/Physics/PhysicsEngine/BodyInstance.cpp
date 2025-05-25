#include "BodyInstance.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Engine/StaticMesh.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/Classes/Engine/SkeletalMesh.h"
#include "Engine/Source/Runtime/CoreUObject/UObject/Casts.h"
#include "Engine/Source/Runtime/Physics/PhysicAsset/BodySetup.h"
#include "Engine/Source/Runtime/Physics/PhysXIntegration.h"
#include "Engine/Source/Runtime/Core/Math/Matrix.h"

void FBodyInstance::CreatePhysXActor()
{
    if (OwnerComponent != nullptr) 
    {
        if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(OwnerComponent))
        {
            CreatePhysXActor(SkeletalMeshComponent);
        }
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(OwnerComponent)) 
        {
            CreatePhysXActor(StaticMeshComponent);
        }
    }
}

void FBodyInstance::CreatePhysXActor(UStaticMeshComponent* OwnerComponent)
{
    // StaticMeshComponent가 가지고 있는 BodySetup을 통해
    // 어떤 형식의 PhysXActor를 어떤 형태로 얼마나 만들지 결정

    // TODO 아래 테스트로 생성하는 코드 수정 필요
    UBodySetup* TestBodySetup = new UBodySetup();
    TestBodySetup->PhysicsType = EPhysicsType::PhysType_Simulated;

    OwnerComponent->GetStaticMesh()->SetBodySetup(TestBodySetup);

    UBodySetup* BodySetup = OwnerComponent->GetStaticMesh()->GetBodySetup();

    if (BodySetup == nullptr) return;

    // 먼저 BodySetup에 오브젝트가 있는지 체크, 아무것도 없다면 
    // AABB를 참고하여 FKBoxElem 한개를 생성해준다.  Physics를 활성화했으므로 합당하다고 생각
    if (BodySetup->AggGeom.GetElementsCount() == 0) 
    {
        FBoundingBox& AABB = OwnerComponent->AABB;
        if (AABB.IsValidBox()) 
        {
            float LengthX = AABB.MaxLocation.X - AABB.MinLocation.X;
            float LengthY = AABB.MaxLocation.Y - AABB.MinLocation.Y;
            float LengthZ = AABB.MaxLocation.Z - AABB.MinLocation.Z;

            float CenterX = AABB.MinLocation.X + LengthX / 2.0f;
            float CenterY = AABB.MinLocation.Y + LengthY / 2.0f;
            float CenterZ = AABB.MinLocation.Z + LengthZ / 2.0f;

            FKBoxElem BoxElem = FKBoxElem(LengthX, LengthY, LengthZ);
            BoxElem.GetTransform().SetTranslation(FVector(CenterX, CenterY, CenterZ));

            BodySetup->AggGeom.BoxElems.Add(BoxElem);
        }
    }

    // BodySetup의 오브젝트들이 1개 이상 있다면
    // PhysXActor를 하나 생성하고 
    // Shape들을 Attach로 추가해서 붙이기
    if (BodySetup->AggGeom.GetElementsCount() <= 0) return;

    FVector Location = OwnerComponent->GetComponentLocation();
    PxVec3 Position = PxVec3(Location.X, Location.Y, Location.Z);

    // PhysXActor로 추가할 GameObject 생성
    GameObject gameObject = CreateGameObject(BodySetup->PhysicsType, Position);
    gameObject.BodyInstance = this;
    PxRigidActor* actor = gameObject.rigidBody;

    // Shape들 추가 작업
    // 1) SphereElem 추가
    for (const FKSphereElem& sph : BodySetup->AggGeom.SphereElems)
    {
        PxSphereGeometry geom(sph.Radius);
        PxShape* shape = gPhysics->createShape(geom, *gMaterial);

        // 로컬 포즈 설정 (Actor 기준)
        const FVector& C = sph.Center;
        shape->setLocalPose(PxTransform(PxVec3(C.X, C.Y, C.Z)));

        actor->attachShape(*shape);
    }

    // 2) BoxElem 추가
    for (const FKBoxElem& box : BodySetup->AggGeom.BoxElems)
    {
        // BoxElem.X/Y/Z는 half‐extent
        PxBoxGeometry geom(PxVec3(box.X * 0.5f, box.Y * 0.5f, box.Z * 0.5f));
        PxShape* shape = gPhysics->createShape(geom, *gMaterial);

        FTransform t = box.GetTransform();
        const FVector& L = t.GetTranslation();
        const FQuat& R = t.GetRotation();
        shape->setLocalPose(
            PxTransform(
                PxVec3(L.X, L.Y, L.Z),
                PxQuat(R.X, R.Y, R.Z, R.W)
            )
        );

        actor->attachShape(*shape);
    }

    // 3) SphylElem(캡슐) 추가
    for (const FKSphylElem& sphyl : BodySetup->AggGeom.SphylElems)
    {
        // Length은 전체 길이, PxCapsuleGeometry은 반높이
        PxCapsuleGeometry geom(sphyl.Radius, sphyl.Length * 0.5f);
        PxShape* shape = gPhysics->createShape(geom, *gMaterial);

        FTransform t = sphyl.GetTransform();
        const FVector& L = t.GetTranslation();
        const FQuat& R = t.GetRotation();
        shape->setLocalPose(
            PxTransform(
                PxVec3(L.X, L.Y, L.Z),
                PxQuat(R.X, R.Y, R.Z, R.W)
            )
        );

        actor->attachShape(*shape);
    }

    // 4) ConvexElem 는 추가 고려 해봐야할듯 일단 구현 패스

    // 8) 동적 시뮬레이션일 때만 질량·관성 계산
    // density는 Material 쪽에서 따로 보관? 해야할듯
    // 일단 10.0f로 하드코딩 되어 있음
    if (BodySetup->PhysicsType == PhysType_Simulated)
    {
        if (PxRigidDynamic* dyn = actor->is< PxRigidDynamic >())
        {
            //PxRigidBodyExt::updateMassAndInertia(*dyn, density);
        }
    }

    // gScene에 추가하는 작업
    gScene->addActor(*actor);
    gObjects.push_back(gameObject);

    gameObject.UpdateFromPhysics();
}

void FBodyInstance::CreatePhysXActor(USkeletalMeshComponent* OwnerComponent)
{

}

void FBodyInstance::UpdateFromPhysics(const physx::PxMat44& InMatrix)
{
    FMatrix Matrix = FMatrix::FromPxMat44(InMatrix);

    OwnerComponent->SetWorldTransform(FTransform(Matrix));

    FVector WorldTranslation = OwnerComponent->GetWorldMatrix().GetTranslationVector();

    //UE_LOG(ELogLevel::Warning, TEXT("WorldTranslation %f %f %f", WorldTranslation.X, WorldTranslation.Y, WorldTranslation.Z));
}
