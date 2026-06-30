// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBatchAsset;
class UObject;

/** 流水线中单个组件的列表项 */
struct FPipelineEntry
{
    UObject*  Component   = nullptr;
    FText     DisplayName;
    bool      bHasError   = false;
    FText     ErrorText;

    bool operator==(const FPipelineEntry& Other) const { return Component == Other.Component; }
};

DECLARE_DELEGATE_OneParam(FOnPipelineSelectionChanged, UObject* /*SelectedComponent*/);

/**
 * 流水线视图
 *
 * 三个分段（Scanner / Filter / Processor），每段：
 *   - 标题行 + "+" 下拉菜单新建
 *   - 列表项（显示类名、校验错误图标）
 *   - 右键 / 选中后可删除
 * 选中某项时触发 OnSelectionChanged 委托，通知 Details 面板更新。
 */
class BATCHPROCESSOREDITOR_API SBatchPipelineView : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchPipelineView) {}
        SLATE_ARGUMENT(TWeakObjectPtr<UBatchAsset>, Asset)
        SLATE_EVENT(FOnPipelineSelectionChanged, OnSelectionChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** 重新从 Asset 读取组件数组，刷新三个列表 */
    void Refresh();

private:
    // ── 列表数据 ──────────────────────────────────────────────────────────────
    using FEntryPtr = TSharedPtr<FPipelineEntry>;

    TArray<FEntryPtr> ScannerEntries;
    TArray<FEntryPtr> FilterEntries;
    TArray<FEntryPtr> ProcessorEntries;

    TSharedPtr<SListView<FEntryPtr>> ScannerList;
    TSharedPtr<SListView<FEntryPtr>> FilterList;
    TSharedPtr<SListView<FEntryPtr>> ProcessorList;

    // ── 辅助 ─────────────────────────────────────────────────────────────────

    TSharedRef<SWidget> BuildSection(const FText& Title,
                                     TArray<FEntryPtr>& Entries,
                                     TSharedPtr<SListView<FEntryPtr>>& OutList,
                                     UClass* ComponentBaseClass,
                                     TFunction<void(int32)> RemoveFn,
                                     TFunction<void(UClass*)> AddFn);

    TSharedRef<ITableRow> GenerateRow(FEntryPtr Item, const TSharedRef<STableViewBase>& OwnerTable,
                                      TFunction<void(int32)> RemoveFn);

    TSharedRef<SWidget> BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn);

    void PopulateEntries(TArray<FEntryPtr>& Out, const TArray<UObject*>& Components);

    void OnScannerAdd(UClass* Class);
    void OnFilterAdd(UClass* Class);
    void OnProcessorAdd(UClass* Class);

    void OnScannerRemove(int32 Index);
    void OnFilterRemove(int32 Index);
    void OnProcessorRemove(int32 Index);

    void OnRowSelected(FEntryPtr Item, ESelectInfo::Type SelectType);

    TWeakObjectPtr<UBatchAsset>     AssetPtr;
    FOnPipelineSelectionChanged     OnSelectionChanged;
};
