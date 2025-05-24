#pragma once
#include "HAL/PlatformType.h"

enum class EAggCollisionShape : int32
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

class FKShapeElem
{
public:
    FKShapeElem(EAggCollisionShape InShapeType) : ShapeType(InShapeType)
    {
        
    }

    // Cast Check용 Static Enum Type (Template에 사용)
    static EAggCollisionShape StaticShapeType;

private:
    EAggCollisionShape ShapeType;
};
