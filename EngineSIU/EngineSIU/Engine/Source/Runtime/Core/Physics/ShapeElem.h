#pragma once
#include "UObject/ObjectMacros.h"
#include "UObject/NameTypes.h"

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
    DECLARE_STRUCT(FKShapeElem)

    FKShapeElem(EAggCollisionShape::Type InShapeType)
    {

    }

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FName , Name, = NAME_None
    )

    EAggCollisionShape::Type ShapeType;

    // !TODO : UserData
    // FUserData UserData;
};
