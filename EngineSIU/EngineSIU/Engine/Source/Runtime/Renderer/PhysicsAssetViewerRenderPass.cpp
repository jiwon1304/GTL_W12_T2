#include "PhysicsAssetViewerRenderPass.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World/PhysicsAssetWorld.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"

FPhysicsAssetViewerRenderPass::FPhysicsAssetViewerRenderPass()
{
    Super::FOverlayShapeRenderPass();
}

FPhysicsAssetViewerRenderPass::~FPhysicsAssetViewerRenderPass()
{
    Super::~FOverlayShapeRenderPass();
}

void FPhysicsAssetViewerRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    Super::Initialize(InBufferManager, InGraphics, InShaderManager);
}

void FPhysicsAssetViewerRenderPass::PrepareRenderArr()
{
    ClearRenderArr();
}

void FPhysicsAssetViewerRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    UPhysicsAssetWorld* PhysicsAssetWorld = Cast<UPhysicsAssetWorld>(GEngine->ActiveWorld);
    if (!PhysicsAssetWorld)
    {
        return;
    }

    USkeletalMeshComponent* SkelComp = PhysicsAssetWorld->GetSkeletalMeshComponent();
    if (!SkelComp)
    {
        return;
    }

    USkeletalMesh* SkeletalMesh = SkelComp->GetSkeletalMeshAsset();
    if (!SkeletalMesh)
    {
        return;
    }

    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
    if (!PhysicsAsset)
    {
        return;
    }

    AssetToRenderArr(SkeletalMesh);

    Super::Render(Viewport);
}

void FPhysicsAssetViewerRenderPass::ClearRenderArr()
{
    Super::ClearRenderArr();
}

void FPhysicsAssetViewerRenderPass::AssetToRenderArr(USkeletalMesh* SkeletalMesh)
{
    // Bone의 정보 얻기
    const FReferenceSkeleton* ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
    const TArray<FMeshBoneInfo>& RawBoneInfo = ReferenceSkeleton->GetRawRefBoneInfo();
    const TArray<FTransform>& RefBonePoses = SkeletalMesh->GetRefSkeleton()->GetRawRefBonePose();

    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
    for (UBodySetup* BodySetup : PhysicsAsset->BodySetup)
    {
        FKAggregateGeom AggGeom = BodySetup->AggGeom;

        //for (const FKSphereElem& SphereElem : AggGeom.SphereElems)
        //{
        //    Shape::FSphere Sphere(SphereElem.Center, SphereElem.Radius);
        //    Spheres.Add(TPair<Shape::FSphere, FLinearColor>(Sphere, FLinearColor(1, 0, 0, 0.2)));
        //}

        //for (const FKBoxElem& BoxElem : AggGeom.BoxElems)
        //{
        //    Shape::FBox Box(BoxElem.Center, BoxElem.X, BoxElem.Y, BoxElem.Z);
        //    Boxes.Add(TPair<Shape::FBox, FLinearColor>(Box, FLinearColor(0, 1, 0, 0.2)));
        //}

        for (const FKSphylElem& SphylElem : AggGeom.SphylElems)
        {
            Shape::FCapsule Capsule = SphylElem.ToFCapsule();
            Capsules.Add(TPair<Shape::FCapsule, FLinearColor>(Capsule, FLinearColor(0, 0, 1, 0.5)));
            Spheres.Add(TPair<Shape::FSphere, FLinearColor>(
                Shape::FSphere(Capsule.A, Capsule.Radius), FLinearColor(1, 0, 0, 0.5)));
            Spheres.Add(TPair<Shape::FSphere, FLinearColor>(
                Shape::FSphere(Capsule.B, Capsule.Radius), FLinearColor(1, 0, 0, 0.5)));
        }
    }
}
