#pragma once
#include "KShapeElem.h"
#include "Math/Vector.h"

struct FKSphylElem : public FKShapeElem
{
    FKSphylElem()
    : FKShapeElem(EAggCollisionShape::Sphyl)
    , Center( FVector::ZeroVector )
    , Rotation(FRotator::ZeroRotator)
    , Radius(1), Length(1)
    {

    }

    FKSphylElem( float InRadius, float InLength )
    : FKShapeElem(EAggCollisionShape::Sphyl)
    , Center( FVector::ZeroVector )
    , Rotation(FRotator::ZeroRotator)
    , Radius(InRadius), Length(InLength)
    {

    }

     virtual ~FKSphylElem() = default;

    
    FVector Center;
    FRotator Rotation;
    float Radius;
    float Length;

    
};
