#pragma once

#include "Define.h"
#include "Engine/Classes/Engine/EngineTypes.h"
#include "Engine/Source/Runtime/Core/Math/Transform.h"

namespace EAggCollisionShape
{
    enum Type : int
    {
        Sphere,
        Box,
        Sphyl,
        Convex,
        TaperedCapsule,
        LevelSet,
        SkinnedLevelSet,

        Unknown
    };
}


struct FKShapeElem
{
    FName Name;
    EAggCollisionShape::Type ShapeType;
    bool bContributeToMass;
    ECollisionEnabled::Type CollisionEnabled;

    // FUserData UserData;

    FKShapeElem()
        : ShapeType(EAggCollisionShape::Unknown)
        , bContributeToMass(true)
        , CollisionEnabled(ECollisionEnabled::QueryAndPhysics)
    {
    }

    FKShapeElem(EAggCollisionShape::Type InShapeType)
        : ShapeType(InShapeType)
        , bContributeToMass(true)
        , CollisionEnabled(ECollisionEnabled::QueryAndPhysics)
    {
    }
};

struct FKSphereElem : public FKShapeElem
{
    FVector Center;
    float Radius;

    FKSphereElem()
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(FVector::ZeroVector)
        , Radius(1)
    {

    }

    FKSphereElem(float r)
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(FVector::ZeroVector)
        , Radius(r) 
    {

    }

    FKSphereElem(float r, FVector InCenter)
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(InCenter)
        , Radius(r) 
    {

    }
};

struct FKBoxElem : public FKShapeElem
{
    FVector Center;
    FRotator Rotation;
    float X;    // Extent of the box
    float Y;
    float Z;

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

    FKBoxElem(float InX, float InY, float InZ)
        :FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , X(InX), Y(InY), Z(InZ)
    {

    }

    FTransform GetTransform() const
    {
        return FTransform(Rotation.Quaternion(), Center);
    }

    void SetTransform(const FTransform& InTransform)
    {
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }
};

struct FKSphylElem : public FKShapeElem 
{
    FVector Center;
    FRotator Rotation;
    float Radius;
    float Length;

    FKSphylElem()
        : FKShapeElem(EAggCollisionShape::Sphyl)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Radius(1), Length(1)
    {

    }

    FKSphylElem(float InRadius, float InLength)
        : FKShapeElem(EAggCollisionShape::Sphyl)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Radius(InRadius), Length(InLength)
    {

    }

    FTransform GetTransform() const 
    {
        return FTransform(Rotation.Quaternion(), Center);
    }

    void SetTransform(const FTransform& InTransform) 
    {
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }
};

struct FKConvexElem : public FKShapeElem 
{
    TArray<FVector> VertexData;
    TArray<int32> IndexData;
    FBoundingBox ElemBox;
    FTransform Transform;

    FTransform GetTransform() const 
    {
        return Transform;
    }

    void SetTransform(const FTransform& InTransform) 
    {
        Transform = InTransform;
    }
};
