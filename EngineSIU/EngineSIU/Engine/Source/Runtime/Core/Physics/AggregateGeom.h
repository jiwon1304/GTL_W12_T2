#pragma once
#include "UObject/ObjectMacros.h"

struct FKAggregateGeom
{
    DECLARE_STRUCT(FKAggregateGeom)

    //UPROPERTY_WITH_FLAGS(
    //    EditAnywhere,
    //    TArray<FVector>, BoxElems, = {}
    //)
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
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        TArray<FKSphereElem>, ConvexElems, = {}
    )
};
