#include "ShaderRegisters.hlsl"

struct VS_INPUT_POS_ONLY
{
    float3 position : POSITION0;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

struct FSphere
{
    float3 Center;
    float Radius;
    float4 Color;
};

cbuffer SphereConstants : register(b11)
{
    FSphere Spheres[512];
}

PS_INPUT SphereVS(VS_INPUT_POS_ONLY input, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;
    
    float3 pos = Spheres[instanceID].Center;
    float scale = Spheres[instanceID].Radius;
    
    float4 localPos = float4(input.position.xyz * scale + pos, 1.f);
        
    localPos = mul(localPos, ViewMatrix);
    localPos = mul(localPos, ProjectionMatrix);
    output.position = localPos;

    output.color = Spheres[instanceID].Color;
    
    return output;
}

float4 SpherePS(PS_INPUT input) : SV_Target
{
    return input.color;
}

struct FCapsule
{
    float3 A; // 캡슐의 한쪽 끝
    float Radius;
    float3 B; // 캡슐의 다른쪽 끝
    float Pad0;
    float4 Color;
};

cbuffer SphereConstants : register(b11)
{
    FCapsule Capsules[512];
}

PS_INPUT CapsuleVS(VS_INPUT_POS_ONLY input, uint VertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;

    // 메시 생성 규칙
    
    const int NumRings = 32;
    const int NumSegments = 32;
    
    float halfRings = NumRings * 0.5f;
    float vertsPerRing = NumSegments + 1;

    // 총 버텍스 개수 계산
    float topHemiVertCount = (halfRings + 1) * vertsPerRing;
    float cylinderVertCount = 2 * vertsPerRing;
    float bottomHemiVertCount = (halfRings + 1) * vertsPerRing;

    // 기본 위치
    float3 meshPos = input.position;

    // 실린더 구간 인덱스
    uint cylinderStart = (uint) topHemiVertCount;
    uint cylinderEnd = cylinderStart + (uint) cylinderVertCount;


    float CapsuleRadius = Capsules[instanceID].Radius;
    float CapsuleHalfHeight = length(Capsules[instanceID].B - Capsules[instanceID].A) * 0.5f;
    float3 CapsuleA = Capsules[instanceID].A;
    float3 CapsuleB = Capsules[instanceID].B;
    
    meshPos *= CapsuleRadius;

    // 메시 생성 구간 판별
    if (VertexID < cylinderStart)
    { // 상단 반구
        meshPos.y += CapsuleHalfHeight;
    }
    else if (VertexID >= cylinderEnd)
    { // 하단 반구
        meshPos.y -= CapsuleHalfHeight;
    }
    else
    { // 실린더 구간
        meshPos.y = -CapsuleHalfHeight;
    }
    
    // 메시 생성 기준: radius=1, halfHeight=1 이라고 가정
    float3 scaledPos = float3(meshPos.x * CapsuleRadius,
                              meshPos.y * CapsuleHalfHeight,
                              meshPos.z * CapsuleRadius);

    // 캡슐의 방향 계산 (A~B)
    float3 CapsuleDir = normalize(CapsuleB - CapsuleA);
    float CapsuleLen = length(CapsuleB - CapsuleA);
    float3 center = (CapsuleA + CapsuleB) * 0.5;

    // Y축 -> CapsuleDir로 회전 (Rodrigues' rotation)
    float3 up = float3(0, 1, 0);
    float3 v = cross(up, CapsuleDir);
    float s = length(v);
    float c = dot(up, CapsuleDir);

    float3x3 rotMatrix;
    if (s < 1e-5)
    {
        rotMatrix = (c > 0) ? float3x3(1, 0, 0, 0, 1, 0, 0, 0, 1)
                            : float3x3(1, 0, 0, 0, -1, 0, 0, 0, -1);
    }
    else
    {
        float vx = v.x, vy = v.y, vz = v.z;
        float vx2 = vx * vx, vy2 = vy * vy, vz2 = vz * vz;
        float3x3 vxmat = float3x3(0, -vz, vy, vz, 0, -vx, -vy, vx, 0);
        rotMatrix =
            float3x3(c + (1 - c) * vx2, (1 - c) * vx * vy - vz * s, (1 - c) * vx * vz + vy * s,
                     (1 - c) * vx * vy + vz * s, c + (1 - c) * vy2, (1 - c) * vy * vz - vx * s,
                     (1 - c) * vx * vz - vy * s, (1 - c) * vy * vz + vx * s, c + (1 - c) * vz2);
    }
    float3 rotated = mul(meshPos, rotMatrix);

    float3 worldPos = center + rotated;

    float4 localPos = float4(worldPos, 1.0f);
    localPos = mul(localPos, ViewMatrix);
    localPos = mul(localPos, ProjectionMatrix);

    output.position = localPos;
    output.color = Capsules[instanceID].Color;

    return output;
}
float4 CapsulePS(PS_INPUT input) : SV_Target
{
    return input.color;
}
