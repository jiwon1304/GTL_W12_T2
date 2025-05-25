#include "AggregateGeom.h"
#include "Math/Transform.h"
void FKAggregateGeom::AddShape(EAggCollisionShape::Type InType, const FTransform& RelativeTransform)
{
    switch (InType)
    {
    case EAggCollisionShape::Sphere:
    {
        FKSphereElem SphereElem;
        SphereElem.SetTransform(RelativeTransform);
        SphereElem.SetRadius(RelativeTransform.GetScale3D().GetMax() * 0.5f);
        SphereElems.Add(SphereElem);
    }
        break;
    case EAggCollisionShape::Box:
    {
        FVector BoxExtent = RelativeTransform.GetScale3D() * 0.5f;
        FKBoxElem BoxElem = FKBoxElem(BoxExtent.X, BoxExtent.Y, BoxExtent.Z);
        BoxElem.SetTransform(RelativeTransform);
        BoxElems.Add(BoxElem);
    }
        break;
    case EAggCollisionShape::Sphyl:
    {
        FKSphylElem SphylElem;
        float Radius = RelativeTransform.GetScale3D().Y * 0.5f;
        float Length = RelativeTransform.GetScale3D().Z;
        SphylElem.SetTransform(RelativeTransform);
        SphylElem.Radius = Radius;
        SphylElem.Length = Length;
        SphylElems.Add(SphylElem);
    }
        break;
        //case EAggCollisionShape::Convex:
        //    ConvexElems.Add(FKConvexElem(RelativeTransform));
        //    break;
    default:
        break;
    }
}

void FKAggregateGeom::Clear()
{
    SphereElems.Empty();
    BoxElems.Empty();
    SphylElems.Empty();
    //ConvexElems.Empty();
}
