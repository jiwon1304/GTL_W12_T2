#pragma once
#include "UObject/ObjectMacros.h"
#include "BoxElem.h"
#include "SphereElem.h"
#include "SphylElem.h"

struct FTransform;
struct FKAggregateGeom
{
    DECLARE_STRUCT(FKAggregateGeom)
    void AddShape(EAggCollisionShape::Type InType, const FTransform& RelativeTransform);
    void Clear();

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        TArray<FKSphereElem>, SphereElems, = {}
    )
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        TArray<FKBoxElem>, BoxElems, = {}
    )
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        TArray<FKSphylElem>, SphylElems, = {}
    )
    //UPROPERTY_WITH_FLAGS(
    //    EditAnywhere,
    //    TArray<FKSphereElem>, ConvexElems, = {}
    //)
};
