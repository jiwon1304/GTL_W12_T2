#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Components/Material/Material.h"
#include "Define.h"

struct FStaticMeshRenderData;

class UStaticMesh : public UObject
{
    DECLARE_CLASS(UStaticMesh, UObject)

public:
    UStaticMesh() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;

    const TArray<FStaticMaterial*>& GetMaterials() const { return Materials; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& OutMaterial) const;
    FStaticMeshRenderData* GetRenderData() const { return RenderData; }

    //ObjectName은 경로까지 포함
    FWString GetOjbectName() const;

    void SetData(FStaticMeshRenderData* InRenderData);

    virtual void SerializeAsset(FArchive& Ar) override;

public:
    /**
     * StaticMesh의 BodySetup을 반환합니다.
     * BodySetup은 물리 시뮬레이션을 위한 설정을 포함합니다.
     */
    class UBodySetup* GetBodySetup() const { return BodySetup; }
    /**
     * StaticMesh의 BodySetup을 설정합니다.
     * @param InBodySetup 설정할 BodySetup
     */
    void SetBodySetup(class UBodySetup* InBodySetup) { BodySetup = InBodySetup; }

private:
    FStaticMeshRenderData* RenderData = nullptr;
    TArray<FStaticMaterial*> Materials;

    UPROPERTY_WITH_FLAGS(
        EditAnywhere | EditInline,
        class UBodySetup*, BodySetup, = nullptr
    )
};
