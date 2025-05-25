#pragma once
#include "BodySetupCore.h"
#include "AggregateGeom.h"

class UBodySetup : public UBodySetupCore
{
    DECLARE_CLASS(UBodySetup, UObject)
public:
    UBodySetup() = default;
    virtual ~UBodySetup() override = default;

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FKAggregateGeom, AggGeom
    )

public:
    void Clear();
};
