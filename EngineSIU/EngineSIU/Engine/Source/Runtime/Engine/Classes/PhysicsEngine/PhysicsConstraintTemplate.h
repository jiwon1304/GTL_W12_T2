#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"
#include "ConstraintInstance.h"

struct FPhysicsConstraintProfileHandle
{
    DECLARE_STRUCT(FPhysicsConstraintProfileHandle)

    UPROPERTY(
        FConstraintProfileProperties,
        ProfileProperties
    )

    UPROPERTY(
        EditAnywhere,
        FName,
        ProfileName,
    )

};

class UPhysicsConstraintTemplate : public UObject
{
    DECLARE_CLASS(UPhysicsConstraintTemplate, UObject)

public:
    UPhysicsConstraintTemplate() = default;

    UPROPERTY(
        EditAnywhere,
        FConstraintInstance, 
        DefaultInstance,
    )

    UPROPERTY(
        TArray<FPhysicsConstraintProfileHandle>,
        ProfileHandles
    )
};
