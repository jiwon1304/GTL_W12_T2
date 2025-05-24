#pragma once
#include "Math/Transform.h"

struct FKSphylElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphylElem)

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, Center
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FRotator, Rotation
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, Radius, = 0.0f
    )
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, Length, = 0.0f
    )

    friend bool operator==(const FKSphylElem& lhs, const FKSphylElem& rhs)
    {
        return lhs.Center == rhs.Center && 
            lhs.Rotation == rhs.Rotation && 
            lhs.Radius == rhs.Radius && 
            lhs.Length == rhs.Length;
    }

    FTransform GetTransform() const
    {
        return FTransform(Rotation, Center);
    }

    void SetTransform(const FTransform& InTransform)
    {
        Center = InTransform.GetTranslation();
        Rotation = InTransform.Rotator();
    }
};
