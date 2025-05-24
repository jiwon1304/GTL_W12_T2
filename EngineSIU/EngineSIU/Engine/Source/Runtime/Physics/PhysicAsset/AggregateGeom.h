#pragma once

#include "FKElements.h"

struct FKAggregateGeom 
{
    TArray<FKSphereElem> SPhereElems;
    TArray<FKBoxElem> BoxElems;
    TArray<FKSphylElem> SPhylElems;
    TArray<FKConvexElem> ConvexElems;
};
