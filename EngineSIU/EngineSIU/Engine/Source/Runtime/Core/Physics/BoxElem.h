#pragma once
#include "Math/Transform.h"
#include "ShapeElem.h"

class FKBoxElem : public FKShapeElem
{
    DECLARE_STRUCT(FKBoxElem)

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
        float, X
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, Y
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, Z
    )

    FKBoxElem()
        :FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , X(1), Y(1), Z(1)
    {

    }

    FKBoxElem(float s)
        :FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , X(s), Y(s), Z(s)
    {

    }

    FKBoxElem(float x, float y, float z)
        :FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , X(x), Y(y), Z(z)
    {
    }

    friend bool operator==(const FKBoxElem& lhs, const FKBoxElem& rhs)
    {
        return lhs.Center == rhs.Center && 
            lhs.Rotation == rhs.Rotation && 
            lhs.X == rhs.X && 
            lhs.Y == rhs.Y && 
            lhs.Z == rhs.Z;
    }

    FTransform GetTransform() const
    {
        return FTransform(Rotation, Center);
    }

    void SetTransform(const FTransform& InTransform)
    {
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }
};
