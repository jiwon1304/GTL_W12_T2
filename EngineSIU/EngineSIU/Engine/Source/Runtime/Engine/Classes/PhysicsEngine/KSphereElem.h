#pragma once
#include "KShapeElem.h"
#include "Math/Vector.h"

struct FKSphereElem : public FKShapeElem
{
    FKSphereElem() 
    : FKShapeElem(EAggCollisionShape::Sphere)
    , Center( FVector::ZeroVector )
    , Radius(1)
    {

    }

    FKSphereElem( float r ) 
    : FKShapeElem(EAggCollisionShape::Sphere)
    , Center( FVector::ZeroVector )
    , Radius(r)
    {

    }

    virtual ~FKSphereElem() = default;
    
    friend bool operator==( const FKSphereElem& LHS, const FKSphereElem& RHS )
    {
        return ( LHS.Center == RHS.Center &&
            LHS.Radius == RHS.Radius );
    }
    
    FVector Center;
    float Radius;
};
