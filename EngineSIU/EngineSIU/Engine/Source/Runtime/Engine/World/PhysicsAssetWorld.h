#pragma once
#include "World.h"


class UPhysicsAssetWorld : public UWorld
{
    DECLARE_CLASS(UPhysicsAssetWorld, UWorld)

public:
    UPhysicsAssetWorld() = default;
        
    static UPhysicsAssetWorld* CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName = "DefaultWorld");
    void Tick(float DeltaTime) override;

    void SetSkeletalMeshComponent(USkeletalMeshComponent* Component)
    {
        SkeletalMeshComponent = Component;
    }
    USkeletalMeshComponent* GetSkeletalMeshComponent()
    {
        return SkeletalMeshComponent;
    }

    int32 SelectBoneIndex = 0;


private:
    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    
};
