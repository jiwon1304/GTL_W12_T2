#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "BodySetup.h"

class UPhysicsAsset : public UObject 
{
    TArray<UBodySetup*> BodySetup;
};
