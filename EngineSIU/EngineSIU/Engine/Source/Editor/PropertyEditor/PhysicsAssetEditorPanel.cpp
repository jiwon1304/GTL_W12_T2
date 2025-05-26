#include "PhysicsAssetEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "World/PhysicsAssetWorld.h"

const float	DefaultPrimSize = 15.0f;
const float	DuplicateXOffset = 10.0f;

FPhysicsAssetEditorPanel::FPhysicsAssetEditorPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::PhysicsAssetEditor);
}

void FPhysicsAssetEditorPanel::Render()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    if (EditorEngine->ActiveWorld == nullptr || EditorEngine->PhysicsAssetEditorWorld == nullptr)
    {
        return;
    }

    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr || BodySetupIconSRV == nullptr || BoxIconSRV == nullptr || SphereIconSRV == nullptr || SphylIconSRV == nullptr)
    {
        LoadBoneIcon();
    }

    /* Pre Setup */
    float TreePanelWidth = (Width) * 0.2f - 6.0f;
    float TreePanelHeight = (Height - 30.f) * 0.5f - 10.f;

    float TreePanelPosX = (Width) * 0.8f + 5.0f;
    float TreePanelPosY = 5.0f;

    ImVec2 TreeMinSize(140, 100);
    ImVec2 TreeMaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(TreeMinSize, TreeMaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(TreePanelPosX, TreePanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(TreePanelWidth, TreePanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::Begin("Bone Hierarchy", nullptr, PanelFlags); // 창 이름 변경
    RenderAddPrimitiveButton();
    ImGui::SameLine();
    RenderPhysicsAssetFilter();

    ImGui::Spacing();
    ImGui::Separator();

    // TODO UISOO

    // Right Click -> Add or ReCreate BodySetup
    //             -> Add Constraint
    //             -> Add Shape
    
    // Delete Key -> Delete
    // Internal -> Add/Remove - BobySetup
    //          -> Add/Remove - Shape
    //          -> Add/Remove - Constraint
    //          -> Select - Bone, BodySetup(Shape), Constraint
    //          -> Delete Key
    
    RenderSkeletonBoneTree();    // Render Bone, Body, Constraint ...

    ImGui::End();

    /* Detail Pre Setup */
    
    float DetailPanelWidth = (Width) * 0.2f - 6.0f;
    float DetailPanelHeight = (Height - 30.f) * 0.5f - 40.f;

    float DetailPanelPosX = (Width) * 0.8f + 5.0f;
    float DetailPanelPosY = (Height - 30.f) * 0.5f;

    ImVec2 DetailMinSize(140, 100);
    ImVec2 DetailMaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(DetailMinSize, DetailMaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(DetailPanelPosX, DetailPanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(DetailPanelWidth, DetailPanelHeight), ImGuiCond_Always);


    ImGui::Begin("Detail", nullptr, PanelFlags); // 창 이름 변경

    RenderDetailPanel();

    ImGui::End();
    
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
    if (ImGui::Button("Exit", ImVec2(ExitPanelWidth, ExitPanelHeight)))
    {
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

void FPhysicsAssetEditorPanel::RenderAddPrimitiveButton()
{
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 ButtonSize = ImVec2(32, 32);
    
    ImGui::PushFont(IconFont);
    if (ImGui::Button("\ue9c8", ButtonSize))
    {
        ImGui::OpenPopup("Add Primitive");
    }
    ImGui::PopFont();

    if (ImGui::BeginPopup("Add Primitive"))
    {
        struct FShape
        {
            const char* Label;
            EAggCollisionShape::Type ShapeType;
        };

        static constexpr FShape Shapes[] =
        {
            {.Label = "Box",      .ShapeType = EAggCollisionShape::Box },
            {.Label = "Sphere",    .ShapeType = EAggCollisionShape::Sphere },
            {.Label = "Capsule", .ShapeType = EAggCollisionShape::Sphyl },
        };

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("Primitive");
        ImGui::PopStyleColor();
        ImGui::Separator();
        for (const auto& Shape : Shapes)
        {
            if (ImGui::Selectable(Shape.Label))
            {
                UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
                if (!EditorEngine)
                {
                    continue;
                }
        
                int32 SelectedBoneIndex = EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex;
                int32 SelectedBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
        
                int32 ParentBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
                int32 SelectedPrimitiveIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
                EAggCollisionShape::Type ShapeType = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;
        
                if (SelectedBoneIndex == -1 && SelectedBodySetupIndex == -1 && (SelectedPrimitiveIndex == -1 || ParentBodySetupIndex == -1))
                {
                    continue;
                }
        
                USkeletalMesh* SkeletalMesh = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
                UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
                
                UBodySetup* SelectedBodySetup = SelectedBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[SelectedBodySetupIndex];
                bool isBoneValid = SelectedBoneIndex == -1 ? false : SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().IsValidRawIndex(SelectedBoneIndex);
                UBodySetup* ParentBodySetup = ParentBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[ParentBodySetupIndex];
                
                bool isPrimitiveExist = false;
                if (ShapeType == EAggCollisionShape::Box)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.BoxElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }
                else if (ShapeType == EAggCollisionShape::Sphere)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphereElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }
                else if (ShapeType == EAggCollisionShape::Sphyl)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphylElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }

                if (!isPrimitiveExist)
                {
                    ParentBodySetup = nullptr;
                }
                
                if (isBoneValid == false && SelectedBodySetup == nullptr && ParentBodySetup == nullptr)
                {
                    continue;
                }

                UBodySetup* TargetBodySetup = nullptr;

                if (isBoneValid)
                {
                    FName BoneName = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetBoneName(SelectedBoneIndex);
                    int32 BodyIndex = PhysicsAsset->FindBodyIndex(BoneName);
                    TargetBodySetup = PhysicsAsset->BodySetup[BodyIndex];
                }
                else if (SelectedBodySetup != nullptr)
                {
                    TargetBodySetup = SelectedBodySetup;
                }
                else if (ParentBodySetup != nullptr)
                {
                    TargetBodySetup = ParentBodySetup;
                }
                
                switch (Shape.ShapeType)
                {
                case EAggCollisionShape::Sphere:
                {
                    FKSphereElem SphereElem = FKSphereElem();
                    TargetBodySetup->AggGeom.SphereElems.Add(SphereElem);
                    SphereElem.Center = FVector::ZeroVector;
                    SphereElem.Radius = DefaultPrimSize;
                    break;
                }
                case EAggCollisionShape::Box:
                {
                    FKBoxElem BoxElem = FKBoxElem();
                    TargetBodySetup->AggGeom.BoxElems.Add(BoxElem);
                    BoxElem.SetTransform( FTransform::Identity );
        
                    BoxElem.X = 0.5f * DefaultPrimSize;
                    BoxElem.Y = 0.5f * DefaultPrimSize;
                    BoxElem.Z = 0.5f * DefaultPrimSize;
                    break;
                }
                case EAggCollisionShape::Sphyl:
                {
                    FKSphylElem SphylElem = FKSphylElem();
                    TargetBodySetup->AggGeom.SphylElems.Add(SphylElem);
        
                    SphylElem.SetTransform( FTransform::Identity );
        
                    SphylElem.Length = DefaultPrimSize;
                    SphylElem.Radius = DefaultPrimSize;
                    
                    break;
                }
                case EAggCollisionShape::Convex:
                case EAggCollisionShape::TaperedCapsule:
                case EAggCollisionShape::LevelSet:
                case EAggCollisionShape::SkinnedLevelSet:
                case EAggCollisionShape::Unknown:
                default:
                    break;
                }
        
                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
            }
        }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::RenderPhysicsAssetFilter()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 ButtonSize = ImVec2(32, 32);
    
    ImGui::PushFont(IconFont);
    if (ImGui::Button("\ue9c4", ButtonSize))
    {
        ImGui::OpenPopup("Physics Asset Filter");
    }
    ImGui::PopFont();
    
    if (ImGui::BeginPopup("Physics Asset Filter"))
    {
        bool ShowBones = EditorEngine->PhysicsAssetEditorWorld->bShowBones;
        if (ImGui::Checkbox("Show Bone", &ShowBones))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBones = ShowBones;
        }

        bool ShowBoneIndices = EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices;
        if (ImGui::Checkbox("Show Bone Index", &ShowBoneIndices))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices = ShowBoneIndices;
        }
        ImGui::Separator();
        bool ShowBodies = EditorEngine->PhysicsAssetEditorWorld->bShowBodies;
        if (ImGui::Checkbox("Show Body", &ShowBodies))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBodies = ShowBodies;
        }
        bool ShowConstraints = EditorEngine->PhysicsAssetEditorWorld->bShowConstraints;
        if (ImGui::Checkbox("Show Constraint", &ShowConstraints))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowConstraints = ShowConstraints;
        }
        bool ShowPrimitives = EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives;
        if (ImGui::Checkbox("Show Primitive", &ShowPrimitives))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives = ShowPrimitives;
        }
        
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::RenderSkeletonBoneTree()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    USkeletalMeshComponent* SelectedSkeletalMeshComponent = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent();
    USkeletalMesh* SelectedSkeletalMesh = SelectedSkeletalMeshComponent->GetSkeletalMeshAsset();
    UPhysicsAsset* SelectedPhysicsAsset = SelectedSkeletalMesh->GetPhysicsAsset();

    int32 BoneIndex = 0;
    if (EditorEngine->PhysicsAssetEditorWorld->bShowBones)
    {
        RenderTreeRecursive(SelectedSkeletalMesh, SelectedPhysicsAsset, BoneIndex, EditorEngine->PhysicsAssetEditorWorld->bShowBones, EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices, EditorEngine->PhysicsAssetEditorWorld->bShowBodies, EditorEngine->PhysicsAssetEditorWorld->bShowConstraints, EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives);
    }
}

void FPhysicsAssetEditorPanel::RenderDetailPanel()
{
    // TODO UISOO
}

void FPhysicsAssetEditorPanel::RenderTreeRecursive(USkeletalMesh* InSkeletalMesh, UPhysicsAsset* InPhysicsAsset, int32 InBoneIndex, uint8 bShowBones, uint8 bShowBoneIndices, uint8 bShowBodies,
    uint8 bShowConstraints, uint8 bShowPrimitives)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    const FReferenceSkeleton* ReferenceSkeleton = InSkeletalMesh->GetRefSkeleton();
    const FMeshBoneInfo& BoneInfo = ReferenceSkeleton->RawRefBoneInfo[InBoneIndex];

    const FString& ShortBoneName = GetCleanBoneName(BoneInfo, InBoneIndex, bShowBoneIndices);

    bool bBoneNodeOpen = false;

    ImGui::PushID(InBoneIndex);

    if (bShowBones)
    {
        ImGui::Image((ImTextureID)BoneIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
        ImGui::SameLine();

        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

        bool bBoneHasChildren = false;
        for (int32 i = 0; i < ReferenceSkeleton->RawRefBoneInfo.Num(); ++i)
        {
            if (ReferenceSkeleton->RawRefBoneInfo[i].ParentIndex == InBoneIndex)
            {
                bBoneHasChildren = true;
                break;
            }
        }

        // int32 BodyIndex = InPhysicsAsset->FindBodyIndex(BoneInfo.Name);
        //
        // if (InPhysicsAsset->BodySetup.Num() > BodyIndex)
        // {
        //     if (bShowBodies)
        //     {
        //         bBoneHasChildren = true;
        //     }
        //     UBodySetup* BodySetup = InPhysicsAsset->BodySetup[BodyIndex];
        //     if (bShowPrimitives && (BodySetup->AggGeom.BoxElems.Num() > 0 || BodySetup->AggGeom.SphereElems.Num() || BodySetup->AggGeom.SphylElems.Num()))
        //     {
        //         bBoneHasChildren = true;
        //     }
        // }

        if (bShowConstraints && InPhysicsAsset->ConstraintSetup.Num() > 0)
        {
            bBoneHasChildren = true;
        }
        
        if (!bBoneHasChildren)
        {
            NodeFlags |= ImGuiTreeNodeFlags_Leaf; // 자식 없으면 리프 노드
            NodeFlags &= ~ImGuiTreeNodeFlags_OpenOnArrow; // 리프 노드는 화살표로 열 필요 없음
        }

        bBoneNodeOpen = ImGui::TreeNodeEx(*ShortBoneName, NodeFlags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
        {
            // 엔진에 선택된 본 인덱스 설정 (가상의 함수 호출)
            EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex = (InBoneIndex);
        }
        ImGui::PopID(); // ID 스택 복원
    }


    if (bBoneNodeOpen) // 노드가 열려있다면
    {
        // TODO
        // BodySetup & Primitive & Constraint
        
        // 자식 본들 재귀적으로 처리
        for (int32 i = 0; i < ReferenceSkeleton->RawRefBoneInfo.Num(); ++i)
        {
            if (ReferenceSkeleton->RawRefBoneInfo[i].ParentIndex == InBoneIndex)
            {
                RenderTreeRecursive(InSkeletalMesh, InPhysicsAsset, i, bShowBones, bShowBoneIndices, bShowBodies, bShowConstraints, bShowPrimitives);
            }
        }
        ImGui::TreePop(); // 트리 노드 닫기
    }
}

FString FPhysicsAssetEditorPanel::GetCleanBoneName(const FMeshBoneInfo& BoneInfo, int32 BoneIndex, uint8 bShowBoneIndices) const
{
    FString InFullName = BoneInfo.Name.ToString();
    // 1) 계층 구분자 '|' 뒤 이름만 취하기
    int32 barIdx = InFullName.FindChar(TEXT('|'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    FString name = (barIdx != INDEX_NONE)
        ? InFullName.RightChop(barIdx + 1)
        : InFullName;

    // 2) 네임스페이스 구분자 ':' 뒤 이름만 취하기
    int32 colonIdx = name.FindChar(TEXT(':'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    if (colonIdx != INDEX_NONE)
    {
        name = name.RightChop(colonIdx + 1);
    }

    if (bShowBoneIndices)
    {
        name += FString::Printf(" <%d>", BoneIndex);
    }
    
    return name;
}

void FPhysicsAssetEditorPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.PNG")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.PNG")->TextureSRV;
    BodySetupIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Body_16x.PNG")->TextureSRV;
    BoxIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/box_16px.PNG")->TextureSRV;
    SphereIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Sphere_16px.PNG")->TextureSRV;
    SphylIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Sphyl_16x.PNG")->TextureSRV;
}
