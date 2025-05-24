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
};

struct FKSphereElem : public FKShapeElem
{
    FVector Center;
    float Radius;
};

struct FKBoxElem : public FKShapeElem
{
    FVector Center;
    FRotator Rotation;
    float X;    // Extent of the box
    float Y;
    float Z;
};

struct FKSphylElem : public FKShapeElem 
{
    FVector Center;
    FRotator Rotation;
    float Radius;
    float Length;
};

struct FKConvexElem : public FKShapeElem 
{
    TArray<FVector> VertexData;
    TArray<int32> IndexData;
    FBoundingBox ElemBox;
    FTransform Transform;
};
