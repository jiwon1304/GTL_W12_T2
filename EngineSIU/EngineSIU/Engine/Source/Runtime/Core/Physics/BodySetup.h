#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UBodySetup : UObject
{
    DECLARE_CLASS(UBodySetup, UObject)
public:
    UBodySetup() = default;
    virtual ~UBodySetup() override = default;
};
