#pragma once

#include "BodySetupCore.h"
#include "AggregateGeom.h"
#include "Engine/Source/Runtime/Physics/PhysicsEngine/PhysicsType.h"

class UBodySetup : public UBodySetupCore 
{
    DECLARE_CLASS(UBodySetup, UBodySetupCore)
    UBodySetup() = default;

    FKAggregateGeom AggGeom;

    EPhysicsType PhysicsType;
};
