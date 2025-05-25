#pragma once
#include "Components/MeshComponent.h"
#include "Engine/StaticMesh.h"

#include "Engine/Asset/StaticMeshAsset.h"
#include "Physics/BodySetup.h"

class UStaticMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

public:
    UStaticMeshComponent() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;


    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;

    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;

    void SetselectedSubMeshIndex(const int& Value) { SelectedSubMeshIndex = Value; }
    int GetselectedSubMeshIndex() const { return SelectedSubMeshIndex; };

    virtual uint32 GetNumMaterials() const override;
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const override;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    virtual TArray<FName> GetMaterialSlotNames() const override;
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const override;

    virtual int CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const override;
    
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    void SetStaticMesh(UStaticMesh* Value);

    // StaticMesh의 BodySetup을 초기화합니다. 이미 존재한다면 Clear하고 사용
    virtual void InitBodySetup() override;
	virtual void ApplyBodySetup(struct FBodyInstance* BodyInstance);

protected:
    UStaticMesh* StaticMesh = nullptr;
    int SelectedSubMeshIndex = -1;
};
