#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"


class UPhysicsAsset : public UObject
{
    DECLARE_CLASS(UPhysicsAsset, UObject)

public:
    UPhysicsAsset() = default;

    class USkeletalMesh* PreviewSkeletalMesh;    
};
