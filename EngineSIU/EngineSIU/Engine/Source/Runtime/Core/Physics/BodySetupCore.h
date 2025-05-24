#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

class UBodySetupCore : public UObject
{
    DECLARE_CLASS(UBodySetupCore, UObject)
public:
    UBodySetupCore() = default;
    virtual ~UBodySetupCore() override = default;

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FName, BoneName, = NAME_None
    )
};
