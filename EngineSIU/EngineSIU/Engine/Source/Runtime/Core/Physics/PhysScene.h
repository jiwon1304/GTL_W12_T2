#pragma once
#include <PxScene.h>
#include <container/Array.h>

using namespace physx;
struct FBodyInstance;

struct FPhysScene
{
    FPhysScene(physx::PxScene* Scene);
    ~FPhysScene();
    // 물리 엔진 업데이트 함수
    void Tick(float DeltaTime);
    // 물리 객체 추가 함수
    FBodyInstance* AddPhysicsObject(class UPrimitiveComponent* Component);
    // 물리 객체 제거 함수
    void RemovePhysicsObject(struct FBodyInstance* BodyToRemove);

    void Simulate(float DeltaTime);


private:
    TArray<FBodyInstance*> PhysicsObjects; // 물리 객체를 저장하는 배열
    physx::PxScene* Scene = nullptr; // PhysX 씬 포인터
    class UWorld* OwningWorld = nullptr; // 월드 포인터
};
