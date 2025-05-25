#pragma once

enum EPhysicsType : int
{
    /** Follow owner.*/
    // PxRigidStatic으로 대응시킬 것임
    PhysType_Default,
    /** Do not follow owner, but make kinematic. */
    // Kinematic PxRigidDynamic 으로 대응시킬 것임
    PhysType_Kinematic,
    /** Do not follow owner, but simulate. */
    // PxRigidDynamic 으로 대응시킬 것임
    PhysType_Simulated
};
