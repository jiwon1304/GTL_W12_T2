#pragma once
#include "KAggregateGeom.h"
#include "PhysicsCore/BodySetupCore.h"


class UBodySetup : public UBodySetupCore
{
    DECLARE_CLASS(UBodySetup, UBodySetupCore)

public:
    UBodySetup() = default;


    
    struct FKAggregateGeom AggGeom;

};
