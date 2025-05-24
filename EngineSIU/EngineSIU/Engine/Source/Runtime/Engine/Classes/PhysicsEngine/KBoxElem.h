#pragma once
#include "KShapeElem.h"
#include "Math/Vector.h"

struct FKBoxElem : public FKShapeElem
{
public:
    
    FKBoxElem()
    : FKShapeElem(EAggCollisionShape::Box)
    , Center( FVector::ZeroVector )
    , Rotation( FRotator::ZeroRotator )
    , Extent(FVector::OneVector)
    {

    }

    FKBoxElem( float s )
    : FKShapeElem(EAggCollisionShape::Box)
    , Center( FVector::ZeroVector )
    , Rotation(FRotator::ZeroRotator)
    , Extent(FVector(s))
    {

    }

    FKBoxElem( float InX, float InY, float InZ ) 
    : FKShapeElem(EAggCollisionShape::Box)
    , Center( FVector::ZeroVector )
    , Rotation(FRotator::ZeroRotator)
    , Extent(FVector(InX, InY, InZ))
    {

    }

    virtual ~FKBoxElem() = default;

    friend bool operator==( const FKBoxElem& LHS, const FKBoxElem& RHS )
    {
        return ( LHS.Center == RHS.Center &&
            LHS.Rotation == RHS.Rotation &&
            LHS.Extent == RHS.Extent );
    }

    FVector Center;
    FRotator Rotation;
    FVector Extent;
};
