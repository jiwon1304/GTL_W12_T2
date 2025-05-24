#pragma once

#include "BodySetupCore.h"
#include "AggregateGeom.h"

class UBodySetup : public UBodySetupCore 
{
    DECLARE_CLASS(UBodySetup, UBodySetupCore)
    UBodySetup() = default;

    FKAggregateGeom AggGeom;
};
