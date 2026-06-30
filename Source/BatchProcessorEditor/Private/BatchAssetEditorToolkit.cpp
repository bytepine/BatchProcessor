// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "BatchAssetEditorToolkit.h"

#include "BatchAsset.h"
#include "ProcessorBase.h"
#include "ConditionBase.h"
#include "EditorBatchProgressReporter.h"
#include "Slate/SBatchPipelineView.h"
#include "Slate/SBatchConsole.h"
#include "Slate/SBatchPreviewList.h"
#include "Slate/SBatchResultLog.h"
#include "Slate/SBatchDiffView.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "FBatchAssetEditorToolkit"

// ── Tab IDs ──────────────────────────────────────────────────────────────────

static const FName PipelineTabId("BatchAsset_Pipeline");
static const FName DetailsTabId("BatchAsset_Details");
static const FName ConsoleTabId("BatchAsset_Console");
static const FName PreviewTabId("BatchAsset_Preview");
static const FName ResultTabId("BatchAsset_Result");
static const FName DiffTabId("BatchAsset_Diff");

// ── 析构 ─────────────────────────────────────────────────────────────────────

FBatchAssetEditorToolkit::~FBatchAssetEditorToolkit()
{
    // 动态多播委托绑定了 lambda，无需手动解绑（lambda 持有 TWeakPtr 会自然失效）
}

// ── 初始化 ───────────────────────────────────────────────────────────────────

void FBatchAssetEditorToolkit::InitBatchAssetEditor(EToolkitMode::Type Mode,
                                                    const TSharedPtr<IToolkitHost>& InitToolkitHost,
                                                    UBatchAsset* InAsset)
{
    check(InAsset);
    BatchAsset = InAsset;

    ProgressReporter = MakeShared<FEditorBatchProgressReporter>();

    // 注：批处理完成事件通过 SBatchConsole::RegisterActiveTimer 检测（running→idle 边界），
    //     由 SpawnTab_Console 的 OnBatchCompleted 回调通知 SBatchResultLog 刷新，无需在此直接绑定。

    // ── 创建 Details 视图 ────────────────────────────────────────────────────
    FPropertyEditorModule& PEM = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bHideSelectionTip    = true;
    DetailsArgs.NotifyHook           = this;
    DetailsArgs.bAllowSearch         = true;
    DetailsView = PEM.CreateDetailView(DetailsArgs);
    // 默认不选中任何对象，等待用户点击流水线节点后再填充

    // 过滤掉子处理器/条件数组——这些由流水线视图内联管理
    // 注意：Instanced 关键字设置的是 CPF_InstancedReference 而非 CPF_PersistentInstance，
    // 因此不检查标志位，直接按元素类型判断。
    DetailsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda(
        [](const FPropertyAndParent& PropertyAndParent) -> bool
        {
            const FArrayProperty* ArrayProp = CastField<FArrayProperty>(&PropertyAndParent.Property);
            if (!ArrayProp) return true;
            const FObjectProperty* InnerProp = CastField<FObjectProperty>(ArrayProp->Inner);
            if (!InnerProp || !InnerProp->PropertyClass) return true;
            // 仅隐藏子处理器数组（由流水线内联管理）
            // 条件数组保留在属性面板中配置
            const UClass* ElemClass = InnerProp->PropertyClass;
            if (ElemClass->IsChildOf(UProcessorBase::StaticClass())) return false;
            return true;
        }));

    // ── 创建各 Slate 组件（延迟到 SpawnTab 中构建完整布局，这里只预分配）─────
    // 实际在 SpawnTab_* 中 SNew

    ExtendToolbar();

    // ── 默认 Tab 布局 ────────────────────────────────────────────────────────
    // 左列（上下分）：预览/结果/差异（上）| 控制台（下）
    // 右列（左右分）：流水线（宽）| 属性（窄）
    TSharedRef<FTabManager::FLayout> Layout =
        FTabManager::NewLayout("BatchAssetEditor_Layout_v4")
        ->AddArea(
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)

            // ── 左列：功能区，上下分割 ────────────────────────────────────
            ->Split(
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Vertical)
                ->SetSizeCoefficient(0.30f)
                // 上：预览 / 结果 / 差异
                ->Split(
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.55f)
                    ->AddTab(PreviewTabId, ETabState::OpenedTab)
                    ->AddTab(ResultTabId,  ETabState::OpenedTab)
                    ->AddTab(DiffTabId,    ETabState::OpenedTab)
                )
                // 下：控制台
                ->Split(
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.45f)
                    ->AddTab(ConsoleTabId, ETabState::OpenedTab)
                )
            )

            // ── 右列：流水线 + 属性，左右分割 ────────────────────────────
            ->Split(
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Horizontal)
                ->SetSizeCoefficient(0.70f)
                // 流水线（核心编辑区，较宽）
                ->Split(
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.65f)
                    ->AddTab(PipelineTabId, ETabState::OpenedTab)
                )
                // 属性（选中组件参数，较窄）
                ->Split(
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.35f)
                    ->AddTab(DetailsTabId, ETabState::OpenedTab)
                )
            )
        );

    constexpr bool bCreateDefaultStandaloneMenu = true;
    constexpr bool bCreateDefaultToolbar        = true;
    FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost,
        FName("BatchAssetEditor"),
        Layout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar,
        InAsset);
}

// ── 工具栏 ───────────────────────────────────────────────────────────────────

void FBatchAssetEditorToolkit::ExtendToolbar()
{
    TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension(
        "Asset", EExtensionHook::After,
        GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP(this, &FBatchAssetEditorToolkit::FillToolbar));
    AddToolbarExtender(ToolbarExtender);
}

void FBatchAssetEditorToolkit::FillToolbar(FToolBarBuilder& Builder)
{
    Builder.BeginSection("BatchActions");

    Builder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::OnRunClicked),
            FCanExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::CanRun)),
        NAME_None,
        LOCTEXT("Run", "运行"),
        LOCTEXT("Run_Tooltip", "执行批处理（实际保存）"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Play"));

    Builder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::OnDryRunClicked),
            FCanExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::CanRun)),
        NAME_None,
        LOCTEXT("DryRun", "试运行"),
        LOCTEXT("DryRun_Tooltip", "试运行（不保存），预览将修改的资产"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visibility"));

    Builder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::OnStopClicked),
            FCanExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::CanStop)),
        NAME_None,
        LOCTEXT("Stop", "停止"),
        LOCTEXT("Stop_Tooltip", "停止正在运行的批处理"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Stop"));

    Builder.AddSeparator();

    Builder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP(this, &FBatchAssetEditorToolkit::OnPreviewClicked)),
        NAME_None,
        LOCTEXT("Preview", "预览资产"),
        LOCTEXT("Preview_Tooltip", "扫描并列出将被处理的候选资产（pre-load，不加载内容）"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Search"));

    Builder.EndSection();
}

void FBatchAssetEditorToolkit::OnRunClicked()
{
    if (UBatchAsset* Asset = BatchAsset.Get())
    {
        Asset->StartWithReporter(ProgressReporter);
    }
}

void FBatchAssetEditorToolkit::OnStopClicked()
{
    if (UBatchAsset* Asset = BatchAsset.Get())
    {
        Asset->Stop();
    }
}

void FBatchAssetEditorToolkit::OnDryRunClicked()
{
    if (UBatchAsset* Asset = BatchAsset.Get())
    {
        Asset->StartWithReporter(ProgressReporter, /*bForceDryRun=*/true);
    }
}

void FBatchAssetEditorToolkit::OnPreviewClicked()
{
    if (PreviewView.IsValid())
    {
        PreviewView->RefreshPreview();
    }
}

bool FBatchAssetEditorToolkit::CanRun() const
{
    const UBatchAsset* Asset = BatchAsset.Get();
    return Asset && !Asset->IsRunning();
}

bool FBatchAssetEditorToolkit::CanStop() const
{
    const UBatchAsset* Asset = BatchAsset.Get();
    return Asset && Asset->IsRunning();
}

// ── Tab 注册 / 注销 ──────────────────────────────────────────────────────────

void FBatchAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
        LOCTEXT("WorkspaceMenu", "批处理编辑器"));

    InTabManager->RegisterTabSpawner(PipelineTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Pipeline))
        .SetDisplayName(LOCTEXT("PipelineTab", "流水线"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());

    InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Details))
        .SetDisplayName(LOCTEXT("DetailsTab", "属性"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());

    InTabManager->RegisterTabSpawner(ConsoleTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Console))
        .SetDisplayName(LOCTEXT("ConsoleTab", "控制台"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());

    InTabManager->RegisterTabSpawner(PreviewTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Preview))
        .SetDisplayName(LOCTEXT("PreviewTab", "预览"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());

    InTabManager->RegisterTabSpawner(ResultTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Result))
        .SetDisplayName(LOCTEXT("ResultTab", "结果"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());

    InTabManager->RegisterTabSpawner(DiffTabId, FOnSpawnTab::CreateSP(this, &FBatchAssetEditorToolkit::SpawnTab_Diff))
        .SetDisplayName(LOCTEXT("DiffTab", "差异"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FBatchAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
    InTabManager->UnregisterTabSpawner(PipelineTabId);
    InTabManager->UnregisterTabSpawner(DetailsTabId);
    InTabManager->UnregisterTabSpawner(ConsoleTabId);
    InTabManager->UnregisterTabSpawner(PreviewTabId);
    InTabManager->UnregisterTabSpawner(ResultTabId);
    InTabManager->UnregisterTabSpawner(DiffTabId);
}

// ── Tab Spawners ─────────────────────────────────────────────────────────────

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Pipeline(const FSpawnTabArgs& Args)
{
    SAssignNew(PipelineView, SBatchPipelineView)
        .Asset(BatchAsset)
        .OnSelectionChanged(FOnPipelineSelectionChanged::CreateSP(
            this, &FBatchAssetEditorToolkit::SelectComponent));

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("PipelineTab", "流水线"))
        [
            PipelineView.ToSharedRef()
        ];
}

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("DetailsTab", "属性"))
        [
            DetailsView.ToSharedRef()
        ];
}

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Console(const FSpawnTabArgs& Args)
{
    SAssignNew(ConsoleView, SBatchConsole)
        .Asset(BatchAsset)
        .Reporter(ProgressReporter)
        .OnBatchCompleted(FOnBatchCompleted::CreateLambda([this](bool bSuccess)
        {
            if (UBatchAsset* Asset = BatchAsset.Get())
            {
                LastDisplayResult = Asset->GetLastResult();
            }
            if (ResultView.IsValid())
            {
                ResultView->SetResult(LastDisplayResult, bSuccess);
            }
        }));

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("ConsoleTab", "控制台"))
        [
            ConsoleView.ToSharedRef()
        ];
}

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Preview(const FSpawnTabArgs& Args)
{
    SAssignNew(PreviewView, SBatchPreviewList)
        .Asset(BatchAsset);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("PreviewTab", "预览"))
        [
            PreviewView.ToSharedRef()
        ];
}

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Result(const FSpawnTabArgs& Args)
{
    SAssignNew(ResultView, SBatchResultLog)
        .Asset(BatchAsset);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("ResultTab", "结果"))
        [
            ResultView.ToSharedRef()
        ];
}

TSharedRef<SDockTab> FBatchAssetEditorToolkit::SpawnTab_Diff(const FSpawnTabArgs& Args)
{
    SAssignNew(DiffView, SBatchDiffView);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        .Label(LOCTEXT("DiffTab", "差异"))
        [
            DiffView.ToSharedRef()
        ];
}

// ── 其他 ─────────────────────────────────────────────────────────────────────

FName FBatchAssetEditorToolkit::GetToolkitFName() const
{
    return FName("BatchAssetEditor");
}

FText FBatchAssetEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("AppLabel", "批处理编辑器");
}

FString FBatchAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
    return TEXT("BatchAsset ");
}

FLinearColor FBatchAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.3f, 0.45f, 0.7f, 0.5f);
}

void FBatchAssetEditorToolkit::RefreshDetails(UObject* SelectedComponent)
{
    if (DetailsView.IsValid())
    {
        if (SelectedComponent)
        {
            DetailsView->SetObject(SelectedComponent);
        }
        else
        {
            DetailsView->SetObject(nullptr);
        }
    }
}

void FBatchAssetEditorToolkit::SelectComponent(UObject* Component)
{
    RefreshDetails(Component);
}

void FBatchAssetEditorToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                                FProperty* PropertyThatChanged)
{
    // 属性面板修改后刷新流水线视图（如增删 Instanced 数组元素）
    if (PipelineView.IsValid())
    {
        PipelineView->Refresh();
    }
    if (ResultView.IsValid())
    {
        ResultView->RefreshValidation();
    }
}

void FBatchAssetEditorToolkit::OnBatchFinished(bool bSuccess)
{
    if (UBatchAsset* Asset = BatchAsset.Get())
    {
        LastDisplayResult = Asset->GetLastResult();
    }
    if (ResultView.IsValid())
    {
        ResultView->SetResult(LastDisplayResult, bSuccess);
    }
}

#undef LOCTEXT_NAMESPACE
