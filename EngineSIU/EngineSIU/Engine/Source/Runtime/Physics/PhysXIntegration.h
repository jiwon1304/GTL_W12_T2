// Integration code for PhysX 4.1
#pragma once

#include <PxPhysicsAPI.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace physx;
using namespace DirectX;

// PhysX 전역
extern PxDefaultAllocator      gAllocator;
extern PxDefaultErrorCallback  gErrorCallback;
extern PxFoundation* gFoundation;
extern PxPhysics* gPhysics;
extern PxScene* gScene;
extern PxMaterial* gMaterial;
extern PxDefaultCpuDispatcher* gDispatcher;

// 게임 오브젝트
struct GameObject {
    PxRigidDynamic* rigidBody = nullptr;
    XMMATRIX worldMatrix = XMMatrixIdentity();

    void UpdateFromPhysics();
};

extern std::vector<GameObject> gObjects;

void InitPhysX();

GameObject CreateBox(const PxVec3& pos, const PxVec3& halfExtents);

void Simulate(float dt);
