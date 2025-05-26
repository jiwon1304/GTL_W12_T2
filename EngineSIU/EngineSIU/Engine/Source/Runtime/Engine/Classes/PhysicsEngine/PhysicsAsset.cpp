#include "PhysicsAsset.h"

int32 UPhysicsAsset::FindBodyIndex(const FName& BoneName) const
{
    const int32* IdxData = BoneNameToIndexMap.Find(BoneName);
    if (IdxData)
    {
        return *IdxData;
    }

    return INDEX_NONE;
}
