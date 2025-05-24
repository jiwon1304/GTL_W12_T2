#pragma once
#include "Math/Transform.h"
struct FKSphereElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphereElem)
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, Radius, = 0.0f
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, Center, = FVector::ZeroVector
    )

    FKSphereElem()
        : Radius(0.0f)
        , Center(FVector::ZeroVector)
    {
        ShapeType = EAggCollisionShape::Sphere;
    }

    void SetRadius(float InRadius)
    {
        Radius = InRadius;
    }

    FTransform GetTransform() const 
    {
        return FTransform(Center);
    }

    void SetTransform(const FTransform& InTransform)
    {
        Center = InTransform.GetTranslation();
    }
};
