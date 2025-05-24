#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UBodySetupCore : public UObject 
{
    DECLARE_CLASS(UBodySetupCore, UObject)
    
    UBodySetupCore() = default;

    FName BoneName;
};
