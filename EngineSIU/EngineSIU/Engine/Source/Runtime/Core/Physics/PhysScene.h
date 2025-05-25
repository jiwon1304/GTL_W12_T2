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
    /** 
    * UPrimitiveComponent를 인자로 받아 PxActor를 생성 및 등록하고 BodyInstance의 초기화 작업을 수행합니다.
    */
    void AddPhysicsObject(class UPrimitiveComponent* Component);
    // 물리 객체 제거 함수
    void RemovePhysicsObject(class UPrimitiveComponent* Component);

    void Simulate(float DeltaTime);

    void StartSimulation();
    void StopSimulation();

private:
    bool bIsSimulating = false;
    TArray<FBodyInstance*> PhysicsObjects; // 물리 객체를 저장하는 배열
    physx::PxScene* Scene = nullptr; // PhysX 씬 포인터
    class UWorld* OwningWorld = nullptr; // 월드 포인터
};
