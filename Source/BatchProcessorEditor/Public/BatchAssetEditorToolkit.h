// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "BatchDefine.h"

class UBatchAsset;
class FEditorBatchProgressReporter;
class SBatchPipelineView;
class SBatchConsole;
class SBatchPreviewList;
class SBatchResultLog;
class SBatchDiffView;
class IDetailsView;

/**
 * 批处理资产专属编辑器
 *
 * 多 Tab 布局：
 *   - 流水线（SBatchPipelineView）：Scanner / Filter / Processor 增删
 *   - 详情（IDetailsView）：选中组件的属性编辑
 *   - 控制台（SBatchConsole）：运行 / 停止 / 试运行 / 进度 / 计数
 *   - 预览（SBatchPreviewList）：pre-load 候选资产列表（P2）
 *   - 结果（SBatchResultLog）：TouchedAssets + 校验横幅（P2）
 *   - 差异（SBatchDiffView）：Dry-run 属性快照（P3）
 */
class BATCHPROCESSOREDITOR_API FBatchAssetEditorToolkit
    : public FAssetEditorToolkit
    , public FNotifyHook
{
public:
    FBatchAssetEditorToolkit() = default;
    virtual ~FBatchAssetEditorToolkit();

    /** 初始化编辑器，由 FBatchAssetTypeActions::OpenAssetEditor 调用 */
    void InitBatchAssetEditor(EToolkitMode::Type Mode,
                              const TSharedPtr<IToolkitHost>& InitToolkitHost,
                              UBatchAsset* InAsset);

    // ── 访问器 ────────────────────────────────────────────────────────────────

    UBatchAsset* GetAsset() const { return BatchAsset.Get(); }
    TSharedPtr<FEditorBatchProgressReporter> GetProgressReporter() const { return ProgressReporter; }

    /** 最近一次批处理完成的结果（供 SBatchResultLog 读取） */
    const FBatchResult& GetLastDisplayResult() const { return LastDisplayResult; }

    /** 流水线有改动时调用，刷新 Details 面板选中状态 */
    void RefreshDetails(UObject* SelectedComponent);

    /** 从流水线视图调用：选中某组件后通知 Details 刷新 */
    void SelectComponent(UObject* Component);

    // ── FAssetEditorToolkit 必须实现 ─────────────────────────────────────────

    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

    // ── FNotifyHook ──────────────────────────────────────────────────────────
    virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                  FProperty* PropertyThatChanged) override;

private:
    // Tab spawner 实现
    TSharedRef<SDockTab> SpawnTab_Pipeline(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Console(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Preview(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Result(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Diff(const FSpawnTabArgs& Args);

    void ExtendToolbar();
    void FillToolbar(FToolBarBuilder& ToolbarBuilder);

    void OnRunClicked();
    void OnStopClicked();
    void OnDryRunClicked();
    void OnPreviewClicked();

    bool CanRun() const;
    bool CanStop() const;

    /** 批处理完成后存档结果并通知结果面板 */
    void OnBatchFinished(bool bSuccess);

    // ── 数据 ─────────────────────────────────────────────────────────────────

    TWeakObjectPtr<UBatchAsset> BatchAsset;
    TSharedPtr<FEditorBatchProgressReporter> ProgressReporter;

    /** 最近一次完成时的结果快照（用于 SBatchResultLog） */
    FBatchResult LastDisplayResult;

    // Tab Widgets
    TSharedPtr<SBatchPipelineView> PipelineView;
    TSharedPtr<IDetailsView>       DetailsView;
    TSharedPtr<SBatchConsole>      ConsoleView;
    TSharedPtr<SBatchPreviewList>  PreviewView;
    TSharedPtr<SBatchResultLog>    ResultView;
    TSharedPtr<SBatchDiffView>     DiffView;
};
