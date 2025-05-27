#include "ShaderRegisters.hlsl"

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

struct VS_INPUT_POS_ONLY
{
    float4 position : POSITION0;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


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

