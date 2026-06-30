// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBatchAsset;
class UObject;
class FArrayProperty;

/**
 * 子组件分组（对应 UObject 上一个 Instanced TArray<UProcessorBase*> 或 TArray<UConditionBase*> 属性）
 */
struct FSubGroupEntry
{
    FArrayProperty*                          ArrayProp   = nullptr;
    FText                                    DisplayName;
    UClass*                                  ElementBase = nullptr; // UProcessorBase 或 UConditionBase
    TArray<TSharedPtr<struct FPipelineEntry>> Items;
};

/** 流水线中单个组件的数据项 */
struct FPipelineEntry
{
    UObject*                Component   = nullptr;
    FText                   DisplayName;
    FString                 Remark;      // 组件备注（反射读取 Remark 属性）
    bool                    bHasError   = false;
    FText                   ErrorText;
    TArray<FSubGroupEntry>  SubGroups;   // 反射发现的子处理器/条件数组

    bool operator==(const FPipelineEntry& Other) const { return Component == Other.Component; }
};

DECLARE_DELEGATE_OneParam(FOnPipelineSelectionChanged, UObject* /*SelectedComponent*/);

/**
 * 节点流图式流水线视图
 *
 * 横向三泳道：[SCANNERS] ──▶ [FILTERS] ──▶ [PROCESSORS]
 * 每个泳道内组件以卡片形式纵向排列，支持：
 *   - 下拉菜单添加组件（按基类枚举所有已注册具体子类）
 *   - 点击卡片选中并驱动右侧 Details 面板（选中态高亮）
 *   - ⚠ 图标标注 ValidateConfig 校验错误
 *   - 含子处理器/条件数组的处理器卡片可展开内联子组
 * 泳道间绘制 ──▶ 箭头表达数据流方向。
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

    TSharedPtr<SVerticalBox> ScannerCardsBox;
    TSharedPtr<SVerticalBox> FilterCardsBox;
    TSharedPtr<SVerticalBox> ProcessorCardsBox;

    // ── 泳道构建 ─────────────────────────────────────────────────────────────

    TSharedRef<SWidget> BuildLane(const FText& Title,
                                  const FLinearColor& HeaderColor,
                                  TArray<FEntryPtr>& Entries,
                                  TSharedPtr<SVerticalBox>& OutCardsBox,
                                  UClass* BaseClass,
                                  TFunction<void(UClass*)> AddFn,
                                  TFunction<void(int32)> RemoveFn);

    TSharedRef<SWidget> BuildArrow() const;
    TSharedRef<SWidget> BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn) const;

    // ── 卡片渲染 ─────────────────────────────────────────────────────────────

    void RebuildCards(SVerticalBox& CardsBox,
                      const TArray<FEntryPtr>& Entries,
                      const FLinearColor& AccentColor,
                      TFunction<void(int32)> RemoveFn);

    TSharedRef<SWidget> BuildCard(FEntryPtr Entry,
                                  int32 Index,
                                  const FLinearColor& AccentColor,
                                  TFunction<void(int32)> RemoveFn);

    /** 构建卡片内联子组区域（子处理器/条件列表） */
    TSharedRef<SWidget> BuildSubGroups(FEntryPtr Entry);

    /** 构建单个子组（标题 + 条目列表 + 添加按钮） */
    TSharedRef<SWidget> BuildSubGroup(FEntryPtr ParentEntry, FSubGroupEntry& Group);

    /** 构建子组中的一行（子组件名 + 删除） */
    TSharedRef<SWidget> BuildSubItem(FEntryPtr ParentEntry,
                                     FSubGroupEntry& Group,
                                     FEntryPtr SubEntry,
                                     int32 SubIndex);

    // ── 数据填充 ─────────────────────────────────────────────────────────────

    void PopulateEntries(TArray<FEntryPtr>& Out, const TArray<UObject*>& Components) const;

    /** 反射扫描 Entry.Component 上的 Instanced 子处理器/条件数组，填充 SubGroups */
    void PopulateSubGroups(FPipelineEntry& Entry) const;

    // ── 添加 / 删除（顶层） ───────────────────────────────────────────────────

    void OnScannerAdd(UClass* Class);
    void OnFilterAdd(UClass* Class);
    void OnProcessorAdd(UClass* Class);

    void OnScannerRemove(int32 Index);
    void OnFilterRemove(int32 Index);
    void OnProcessorRemove(int32 Index);

    // ── 添加 / 删除（子组件） ─────────────────────────────────────────────────

    void OnSubComponentAdd(UObject* Parent, FArrayProperty* ArrayProp, UClass* Class);
    void OnSubComponentRemove(UObject* Parent, FArrayProperty* ArrayProp, int32 Index);

    // ── 选中 ─────────────────────────────────────────────────────────────────

    void SelectComponent(UObject* Component);

    /**
     * 展开/折叠状态：使用 UObject* 作 key，默认展开。
     * 用 TSet 记录"已折叠"的组件，Refresh 后仍有效。
     */
    TSet<UObject*>              CollapsedComponents;

    TWeakObjectPtr<UObject>     SelectedComponent;
    TWeakObjectPtr<UBatchAsset> AssetPtr;
    FOnPipelineSelectionChanged OnSelectionChanged;

    static const FLinearColor ScannerColor;
    static const FLinearColor FilterColor;
    static const FLinearColor ProcessorColor;
};
