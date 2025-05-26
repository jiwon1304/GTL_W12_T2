#include "ShaderRegisters.hlsl"

Texture2D SceneDepthTexture : register(t99);
Texture2D SceneTexture : register(t100);
SamplerState SceneSampler : register(s0);

cbuffer DOFConstantBuffer : register(b0)
{
    float FocalDistance;
    float FocalRegion;
    float BlurAmount;
    float BlurRadius;
};

// 단순한 quad
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float3 ReconstructWorldPos(float2 UV, float Depth)
{
    float4 NDC;
    NDC.xy = UV * 2.0 - 1.0; // [0,1] → [-1,1]
    NDC.y *= -1;
    NDC.z = Depth;
    NDC.w = 1.0;

    float4 ViewPos = mul(NDC, InvProjectionMatrix);
    ViewPos /= ViewPos.w;

    return ViewPos.xyz;
}

// Depth 텍스처가 있고, 해당 픽셀의 depth 값을 구하는 함수가 있다고 가정
float GetSceneDepth(float2 uv)
{
    // 여기서 DSV는 R24G8
    float NDCDepth = SceneDepthTexture.Sample(SceneSampler, uv).r;
    return ReconstructWorldPos(uv, NDCDepth).z;
}

// focus 영역으로부터 멀면 1.0, focus 영역이면 0을 리턴하는 함수
float GetFocusFactor(float centerDepth, float FocalDistance, float FocalRegion, float Multiplier)
{
    // 포커스 영역: [FocalDistance, FocalDistance + FocalRegion]
    if (centerDepth >= FocalDistance && centerDepth <= FocalDistance + FocalRegion)
    {
        return 0.0; // 완전한 포커스(블러 없음)
    }
    else if (centerDepth < FocalDistance)
    {
        // 포커스 영역보다 카메라 쪽이면, 포커스 영역과의 거리만큼 blur 증가
        return saturate((FocalDistance - centerDepth) / FocalRegion * Multiplier);
    }
    else // centerDepth > FocalDistance + FocalRegion
    {
        // 포커스 영역보다 멀면, 포커스 영역과의 거리만큼 blur 증가
        return saturate((centerDepth - (FocalDistance + FocalRegion)) / FocalRegion * Multiplier);
    }
}

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    // blur된 값 구하기
    int k = int(BlurRadius);
    
    float3 FinalColor = float3(0, 0, 0);
    float2 uv = input.UV;

    for (int i = -k; i <= k; ++i)
    {
        for (int j = -k; j <= k; ++j)
        {
            float2 uvAdj = uv + float2(i, j) / 1080.0;
            float3 Color = SceneTexture.Sample(SceneSampler, uvAdj).rgb;
            FinalColor += Color;
        }
    }
    FinalColor /= (2 * k + 1) * (2 * k + 1);

    // 현재 픽셀의 깊이를 통해서 블러 정도를 계산
    float CenterDepth = GetSceneDepth(uv);
    
    float FocusFactor = GetFocusFactor(CenterDepth, FocalDistance, FocalRegion, BlurAmount);
    
    return float4(FinalColor, FocusFactor);
}

VS_OUTPUT
    mainVS(
    uint vertexId : SV_VertexID)
{
    VS_OUTPUT output;

    // 6개의 정점으로 2개의 삼각형(쿼드) 그리기
    // 정점 인덱스별 위치와 UV 좌표
    float2 positions[6] =
    {
        float2(-1.0, -1.0), // bottom left
        float2(-1.0, 1.0), // top left
        float2(1.0, -1.0), // bottom right
        float2(-1.0, 1.0), // top left
        float2(1.0, 1.0), // top right
        float2(1.0, -1.0) // bottom right
    };
    float2 uvs[6] =
    {
        float2(0.0, 1.0),
        float2(0.0, 0.0),
        float2(1.0, 1.0),
        float2(0.0, 0.0),
        float2(1.0, 0.0),
        float2(1.0, 1.0)
    };

    output.Position = float4(positions[vertexId], 0.0, 1.0);
    output.UV = uvs[vertexId];
    return output;
}
