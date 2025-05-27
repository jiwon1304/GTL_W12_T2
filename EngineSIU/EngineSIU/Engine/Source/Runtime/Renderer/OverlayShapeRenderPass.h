#pragma once
#include "IRenderPass.h"
#include "Container/Array.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "Math/Shapes.h"
#include "Math/Color.h"

class FOverlayShapeRenderPass : public IRenderPass
{
public:
    FOverlayShapeRenderPass();
    virtual ~FOverlayShapeRenderPass() override;
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    virtual void PrepareRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void ClearRenderArr() override;

private:
    void CreateShaders();
    void CreateBlendState();
    void CreateBuffers();
    void CreateConstants();

    void StartRender(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void EndRender(const std::shared_ptr<FEditorViewportClient>& Viewport);
private:
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;
    
    TArray< TPair<Shape::FRay, FLinearColor> > Rays;
    TArray< TPair<Shape::FSphere, FLinearColor> > Spheres;
    TArray< TPair<Shape::FBox, FLinearColor> > Boxes;
    TArray< TPair<Shape::FOrientedBox, FLinearColor> > OrientedBoxes;
    TArray< TPair<Shape::FCapsule, FLinearColor> > Capsules;
    TArray< TPair<Shape::FPlane, FLinearColor> > Planes;
    TArray< TPair<Shape::FCone, FLinearColor> > Cones;

    constexpr static int32 ConstantBufferSize = 512;

    ID3D11BlendState* AlphaBlendState;
    ID3D11DepthStencilState* NoZWriteState;
};
