#pragma once
#include "Container/Array.h"

struct FKSphylElem;
struct FKBoxElem;
struct FKSphereElem;

struct FKAggregateGeom
{
    TArray<FKSphereElem> SphereElems;
    TArray<FKBoxElem> BoxElems;
    TArray<FKSphylElem> SphylElems;
    // TArray<FKConvexElem> ConvexElems;
};
