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

// Depth 텍스처가 있고, 해당 픽셀의 depth 값을 구하는 함수가 있다고 가정
float GetSceneDepth(float2 uv)
{
    // 여기서 DSV는 R24G8
    return SceneDepthTexture.Sample(SceneSampler, uv).r;
}

float GetFocusFactor(float centerDepth, float FocalDistance, float FocalRegion)
{
    // 포커스 영역: [FocalDistance, FocalDistance + FocalRegion]
    if (centerDepth >= FocalDistance && centerDepth <= FocalDistance + FocalRegion)
    {
        return 0.0; // 완전한 포커스(블러 없음)
    }
    else if (centerDepth < FocalDistance)
    {
        // 포커스 영역보다 카메라 쪽이면, 포커스 영역과의 거리만큼 blur 증가
        return saturate((FocalDistance - centerDepth) / FocalRegion);
    }
    else // centerDepth > FocalDistance + FocalRegion
    {
        // 포커스 영역보다 멀면, 포커스 영역과의 거리만큼 blur 증가
        return saturate((centerDepth - (FocalDistance + FocalRegion)) / FocalRegion);
    }
}

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.UV;
    // 현재 픽셀의 뎁스(가정: 0~1 range)
    float centerDepth = GetSceneDepth(uv);

    float focusFactor = GetFocusFactor(centerDepth, FocalDistance, FocalRegion);
    float blurStrength = focusFactor * BlurAmount;
    float radius = lerp(0, BlurRadius, blurStrength);
    blurStrength = saturate(blurStrength);
    
    radius = 10;
    blurStrength = 0.5;

    // 간단한 원형 블러 샘플링
    float3 colorSum = 0;
    float weightSum = 0;
    const int sampleCount = 12;
    for (int i = 0; i < sampleCount; ++i)
    {
        float angle = 2 * 3.141592 * (i / (float) sampleCount);
        float2 offset = float2(cos(angle), sin(angle)) * radius;
        float2 sampleUV = uv + offset;
        float3 sampleColor = SceneTexture.Sample(SceneSampler, sampleUV).rgb;

        // 간단히 가중치 1, 실제로는 가우시안 등 가중치 적용 가능
        colorSum += sampleColor;
        weightSum += 1.0;
    }

    float3 blurredColor = colorSum / weightSum;
    // 포커스 강도에 따라 원본/블러드 색 혼합
    float3 originalColor = SceneTexture.Sample(SceneSampler, uv).rgb;
    float3 finalColor = lerp(originalColor, blurredColor, blurStrength);

    return float4(finalColor, 1.0);
}





VS_OUTPUT mainVS(uint vertexId : SV_VertexID)
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
