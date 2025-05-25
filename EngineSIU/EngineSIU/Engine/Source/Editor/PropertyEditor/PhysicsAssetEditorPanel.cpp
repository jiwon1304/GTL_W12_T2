#include "PhysicsAssetEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "Engine/SkeletalMesh.h"
#include "World/PhysicsAssetWorld.h"

FPhysicsAssetEditorPanel::FPhysicsAssetEditorPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::PhysicsAssetEditor);
}

void FPhysicsAssetEditorPanel::Render()
{
    // TODO UISOO Implement (늦어도 ㄱㅊ)
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    if (EditorEngine->ActiveWorld == nullptr)
    {
        return;
    }

    USkeleton* Skeleton = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset()->GetSkeleton();
    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr) {
        LoadBoneIcon();
    }

    /* Pre Setup */
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.7f;

    float PanelPosX = (Width) * 0.8f+5.0f;
    float PanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    // TODO UISOO Implement First

    // Draw

    RenderAddButton();
    RenderPhysicsAssetFilter();

    // Delete Key -> Delete
    // Internal -> Add/Remove - BobySetup
    //          -> Add/Remove - Shape
    //          -> Add/Remove - Constraint
    //          -> Select - Bone, BodySetup(Shape), Constraint
    
    
    // RenderSkeletonBoneTree();    // Render Bone, Body, Constraint ...
    
    float ExitPanelWidth = (Width) * 0.2f - 6.0f;
    float ExitPanelHeight = 30.0f;

    float ExitPanelPosX = Width - ExitPanelWidth;
    float ExitPanelPosY = Height - ExitPanelHeight - 10;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::SetNextWindowSize(ImVec2(ExitPanelWidth, ExitPanelHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ExitPanelPosX, ExitPanelPosY), ImGuiCond_Always);

    constexpr ImGuiWindowFlags ExitPanelFlags =
        ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoScrollbar;
        
    ImGui::Begin("Exit Physics Asset Editor", nullptr, ExitPanelFlags);
    if (ImGui::Button("Exit", ImVec2(ExitPanelWidth, ExitPanelHeight))) {
        // TODO UISOO Check
        // ClearRefSkeletalMeshComponent();
        EditorEngine->EndPhysicsAssetEditor();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void FPhysicsAssetEditorPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    if (hWnd && GetClientRect(hWnd, &clientRect))
    {
        Width = static_cast<float>(clientRect.right - clientRect.left);
        Height = static_cast<float>(clientRect.bottom - clientRect.top);
    }
}

void FPhysicsAssetEditorPanel::RenderAddButton()
{
    // Add Shape, Constraint -> To Selected
    
}

void FPhysicsAssetEditorPanel::RenderSkeletonBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex)
{
}

void FPhysicsAssetEditorPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.PNG")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.PNG")->TextureSRV;
}
