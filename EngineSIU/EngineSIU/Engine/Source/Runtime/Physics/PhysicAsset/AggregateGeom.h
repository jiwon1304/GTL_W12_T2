#pragma once

#include "FKElements.h"

struct FKAggregateGeom 
{
    TArray<FKSphereElem> SphereElems;
    TArray<FKBoxElem> BoxElems;
    TArray<FKSphylElem> SphylElems;
    TArray<FKConvexElem> ConvexElems;

    inline uint32 GetElementsCount() 
    {
        return (SphereElems.Num() + BoxElems.Num() + SphylElems.Num() + ConvexElems.Num());
    }
};
