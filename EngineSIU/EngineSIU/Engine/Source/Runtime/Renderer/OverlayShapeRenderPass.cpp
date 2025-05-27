#include "OverlayShapeRenderPass.h"
#include <algorithm>
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealClient.h"
namespace Constants
{
    struct alignas(16) Ray
    {
        FVector Origin;
        float Pad0;
        FVector Direction;
        float Pad1;
        FLinearColor Color;
    };

    struct alignas(16)  Sphere
    {
        FVector Center;
        float Radius;
        FLinearColor Color;
    };

    struct alignas(16) Box
    {
        FVector Min;
        float Pad0;
        FVector Max;
        float Pad1;
        FLinearColor Color;
    };

    struct alignas(16) OrientedBox
    {
        FVector AxisX;
        float ExtentX;
        FVector AxisY;
        float ExtentY;
        FVector AxisZ;
        float ExtentZ;
        FVector Center;
        float Pad0;
        FLinearColor Color;
    };

    struct alignas(16) Capsule
    {
        FVector A; // 캡슐의 한쪽 끝
        float Radius;
        FVector B; // 캡슐의 다른쪽 끝
        float Pad0;
        FLinearColor Color;
    };

    struct alignas(16) Plane
    {
        float X, Y, Z, W; // 평면의 방정식 계수
        FLinearColor Color;
    };

    struct alignas(16) Cone
    {
        FVector ApexPosition; // 원뿔의 꼭지점 위치
        float Radius;         // 원뿔의 밑면 반지름
        FVector Direction;    // 원뿔의 방향 벡터
        float Angle;          // 원뿔의 각도 (라디안 단위)
        FLinearColor Color;
    };
}

FOverlayShapeRenderPass::FOverlayShapeRenderPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
{
}

FOverlayShapeRenderPass::~FOverlayShapeRenderPass()
{
}

void FOverlayShapeRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;
    CreateShaders();
    CreateBuffers();
    CreateConstants();
}

void FOverlayShapeRenderPass::PrepareRenderArr()
{
    ClearRenderArr();
    
    Spheres.Add(TPair<Shape::FSphere, FLinearColor>(Shape::FSphere(FVector(0, 0, 0), 10.f), FLinearColor(1,0,0,0.5)));
}

void FOverlayShapeRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    StartRender(Viewport);
    FVector CameraPosition = Viewport->GetCameraLocation();

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if (Spheres.Num() > 0)
    {
        // Shader Setup
        ShaderManager->SetVertexShaderAndInputLayout(L"OverlayShapeVertexShaderSphere", Graphics->DeviceContext);
        ShaderManager->SetPixelShader(L"OverlayShapePixelShaderSphere", Graphics->DeviceContext);

        // Vertex Index Buffer
        const FVertexInfo& VertexInfo = BufferManager->GetVertexBuffer(L"OverlaySphereVertexBuffer");
        UINT Offset = 0;
        Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &VertexInfo.Stride, &Offset);
        const FIndexInfo& IndexInfo = BufferManager->GetIndexBuffer(L"OverlaySphereIndexBuffer");
        Graphics->DeviceContext->IASetIndexBuffer(IndexInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // Constant Buffer
        // sorting 필요 없음. 그냥 alpha만 가산해서 그림.
        //std::sort(Spheres.begin(), Spheres.end(),
        //    [&CameraPosition](const std::pair<Shape::FSphere, FLinearColor>& A, const std::pair<Shape::FSphere, FLinearColor>& B) {
        //        return FVector::DistSquared(CameraPosition, A.first.Center) > FVector::DistSquared(CameraPosition, B.first.Center);
        //    }
        //);
        TArray<Constants::Sphere> SphereConstants;
        SphereConstants.SetNum(Spheres.Num());
        for (int Index = 0; Index < Spheres.Num(); ++Index)
        {
            const Shape::FSphere Sphere = Spheres[Index].Key;
            const FLinearColor Color = Spheres[Index].Value; 
            SphereConstants[Index].Center = Sphere.Center;
            SphereConstants[Index].Radius = Sphere.Radius;
            SphereConstants[Index].Color = Color;
        }

        BufferManager->BindConstantBuffer("SphereConstantBuffer", 11, EShaderStage::Vertex);
        BufferManager->BindConstantBuffer("SphereConstantBuffer", 11, EShaderStage::Pixel);
        int BufferIndex = 0;
        for (int i = 0; i < (1 + SphereConstants.Num() / ConstantBufferSize) * ConstantBufferSize; ++i)
        {
            TArray<Constants::Sphere> SubBuffer;


            for (int j = 0; j < ConstantBufferSize; ++j)
            {
                if (BufferIndex < SphereConstants.Num())
                {
                    SubBuffer.Add(SphereConstants[BufferIndex]);
                    ++BufferIndex;
                }
                else
                {
                    break;
                }
            }

            if (SubBuffer.Num() > 0)
            {
                BufferManager->UpdateConstantBuffer<Constants::Sphere>("SphereConstantBuffer", SubBuffer);
                Graphics->DeviceContext->DrawIndexedInstanced(IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
            }
        }
    }
    EndRender(Viewport);
}

void FOverlayShapeRenderPass::ClearRenderArr()
{
    Rays.Empty();
    Spheres.Empty();
    Boxes.Empty();
    OrientedBoxes.Empty();
    Capsules.Empty();
    Planes.Empty();
    Cones.Empty();
}

void FOverlayShapeRenderPass::CreateShaders()
{
    D3D11_INPUT_ELEMENT_DESC PositionOnly[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ShaderManager->AddVertexShaderAndInputLayoutAsync(L"OverlayShapeVertexShaderSphere", L"Shaders/OverlayShapeShader.hlsl", "SphereVS", PositionOnly, 1, nullptr);
    ShaderManager->AddPixelShaderAsync(L"OverlayShapePixelShaderSphere", L"Shaders/OverlayShapeShader.hlsl", "SpherePS", nullptr);
}

void FOverlayShapeRenderPass::CreateBlendState()
{
    //// === [1] Alpha Blend State 생성 ===
    //D3D11_BLEND_DESC BlendDesc = {};
    //BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    //BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    //BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    //BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    //BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //Graphics->Device->CreateBlendState(&BlendDesc, &AlphaBlendState);

    // === [2] DepthStencil State (Z-Write Off) 생성 ===
    D3D11_DEPTH_STENCIL_DESC DepthDesc = {};
    DepthDesc.DepthEnable = false;  
    //DepthDesc.DepthEnable = TRUE;
    //DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    //DepthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    Graphics->Device->CreateDepthStencilState(&DepthDesc, &NoZWriteState);
}

void FOverlayShapeRenderPass::CreateBuffers()
{
    TArray<FVector> SphereVertices;
    TArray<uint32> SphereIndices;
    const int NumSegments = 32;
    const int NumRings = 32;
    // 버텍스 생성
    for (int ring = 0; ring <= NumRings; ++ring)
    {
        float phi = ring * PI / NumRings; // 0 ~ PI
        float y = std::cos(phi); // y축

        for (int seg = 0; seg <= NumSegments; ++seg)
        {
            float theta = seg * 2.0f * PI / NumSegments; // 0 ~ 2PI
            float x = std::sin(phi) * std::cos(theta);
            float z = std::sin(phi) * std::sin(theta);
            SphereVertices.Emplace(FVector(x, y, z));
        }
    }

    // 인덱스 생성 (삼각형 리스트)
    for (int ring = 0; ring < NumRings; ++ring)
    {
        for (int seg = 0; seg < NumSegments; ++seg)
        {
            int curr = ring * (NumSegments + 1) + seg;
            int next = (ring + 1) * (NumSegments + 1) + seg;

            // 삼각형 1
            SphereIndices.Add(curr);
            SphereIndices.Add(next);
            SphereIndices.Add(curr + 1);

            // 삼각형 2
            SphereIndices.Add(next);
            SphereIndices.Add(next + 1);
            SphereIndices.Add(curr + 1);
        }
    }
    FVertexInfo SphereVertexInfo;
    BufferManager->CreateVertexBuffer<FVector>("OverlaySphereVertexBuffer", SphereVertices, SphereVertexInfo, D3D11_USAGE_IMMUTABLE, 0);
    FIndexInfo SphereIndexInfo;
    BufferManager->CreateIndexBuffer<uint32>("OverlaySphereIndexBuffer", SphereIndices, SphereIndexInfo, D3D11_USAGE_IMMUTABLE, 0);
}

void FOverlayShapeRenderPass::CreateConstants()
{
    BufferManager->CreateBufferGeneric<int>("RayConstantBuffer", nullptr, sizeof(Constants::Ray) * ConstantBufferSize, 
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("SphereConstantBuffer", nullptr, sizeof(Constants::Sphere) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("BoxConstantBuffer", nullptr, sizeof(Constants::Box) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("OrientedBoxConstantBuffer", nullptr, sizeof(Constants::OrientedBox) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("CapsuleConstantBuffer", nullptr, sizeof(Constants::Capsule) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("PlaneConstantBuffer", nullptr, sizeof(Constants::Plane) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<int>("ConeConstantBuffer", nullptr, sizeof(Constants::Cone) * ConstantBufferSize,
        D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FOverlayShapeRenderPass::StartRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    constexpr EResourceType ResourceType = EResourceType::ERT_PP_ShapeOverlay;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    const FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    //float BlendFactor[4] = { 0, 0, 0, 0 };
    //Graphics->DeviceContext->OMSetBlendState(AlphaBlendState, BlendFactor, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(NoZWriteState, 0);
}

void FOverlayShapeRenderPass::EndRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    //Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(nullptr, 0);
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}
