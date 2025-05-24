#pragma once
#include "UObject/ObjectMacros.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Components/PrimitiveComponent.h"

class UPrimitiveComponent;

struct FBodyInstance
{
    DECLARE_STRUCT(FBodyInstance)
    UPrimitiveComponent* OwnerComponent;
};
