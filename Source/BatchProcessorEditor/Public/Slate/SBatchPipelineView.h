// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBatchAsset;
class UObject;

/** 流水线中单个组件的数据项 */
struct FPipelineEntry
{
    UObject* Component   = nullptr;
    FText    DisplayName;
    bool     bHasError   = false;
    FText    ErrorText;

    bool operator==(const FPipelineEntry& Other) const { return Component == Other.Component; }
};

DECLARE_DELEGATE_OneParam(FOnPipelineSelectionChanged, UObject* /*SelectedComponent*/);

/**
 * 节点流图式流水线视图
 *
 * 横向三泳道：[SCANNERS] ──▶ [FILTERS] ──▶ [PROCESSORS]
 * 每个泳道内组件以卡片形式纵向排列，支持：
 *   - 下拉菜单添加组件（按基类枚举所有已注册具体子类）
 *   - 悬停显示删除按钮
 *   - 点击卡片选中并驱动右侧 Details 面板
 *   - ⚠ 图标标注 ValidateConfig 校验错误
 * 两侧泳道间绘制 ──▶ 箭头表达数据流方向。
 */
class BATCHPROCESSOREDITOR_API SBatchPipelineView : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchPipelineView) {}
        SLATE_ARGUMENT(TWeakObjectPtr<UBatchAsset>, Asset)
        SLATE_EVENT(FOnPipelineSelectionChanged, OnSelectionChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** 重新从 Asset 读取组件数组，刷新所有泳道 */
    void Refresh();

private:
    using FEntryPtr = TSharedPtr<FPipelineEntry>;

    // ── 泳道数据 ──────────────────────────────────────────────────────────────
    TArray<FEntryPtr> ScannerEntries;
    TArray<FEntryPtr> FilterEntries;
    TArray<FEntryPtr> ProcessorEntries;

    // 三个泳道 slot holders（SVerticalBox 中的卡片区域）
    TSharedPtr<SVerticalBox> ScannerCardsBox;
    TSharedPtr<SVerticalBox> FilterCardsBox;
    TSharedPtr<SVerticalBox> ProcessorCardsBox;

    // ── 泳道构建 ─────────────────────────────────────────────────────────────

    /** 构建一个完整泳道（标题头 + 卡片区 + 添加按钮） */
    TSharedRef<SWidget> BuildLane(const FText& Title,
                                  const FLinearColor& HeaderColor,
                                  TArray<FEntryPtr>& Entries,
                                  TSharedPtr<SVerticalBox>& OutCardsBox,
                                  UClass* BaseClass,
                                  TFunction<void(UClass*)> AddFn,
                                  TFunction<void(int32)> RemoveFn);

    /** 泳道间的 ──▶ 箭头连接符 */
    TSharedRef<SWidget> BuildArrow() const;

    /** 添加组件下拉菜单按钮 */
    TSharedRef<SWidget> BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn) const;

    // ── 卡片渲染 ─────────────────────────────────────────────────────────────

    /** 重建某个泳道内的所有卡片 Widget */
    void RebuildCards(SVerticalBox& CardsBox,
                      const TArray<FEntryPtr>& Entries,
                      TFunction<void(int32)> RemoveFn);

    /** 构建单张组件卡片 */
    TSharedRef<SWidget> BuildCard(FEntryPtr Entry,
                                  int32 Index,
                                  const FLinearColor& AccentColor,
                                  TFunction<void(int32)> RemoveFn);

    // ── 数据填充 ─────────────────────────────────────────────────────────────

    void PopulateEntries(TArray<FEntryPtr>& Out, const TArray<UObject*>& Components) const;

    // ── 添加 / 删除 ──────────────────────────────────────────────────────────

    void OnScannerAdd(UClass* Class);
    void OnFilterAdd(UClass* Class);
    void OnProcessorAdd(UClass* Class);

    void OnScannerRemove(int32 Index);
    void OnFilterRemove(int32 Index);
    void OnProcessorRemove(int32 Index);

    // ── 选中 ─────────────────────────────────────────────────────────────────

    void SelectEntry(FEntryPtr Entry);

    /** 当前选中项（高亮） */
    TWeakPtr<FPipelineEntry>        SelectedEntry;

    TWeakObjectPtr<UBatchAsset>     AssetPtr;
    FOnPipelineSelectionChanged     OnSelectionChanged;

    // 每种类型的颜色
    static const FLinearColor ScannerColor;
    static const FLinearColor FilterColor;
    static const FLinearColor ProcessorColor;
};
