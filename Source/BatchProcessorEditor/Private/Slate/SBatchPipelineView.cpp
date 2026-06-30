// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchPipelineView.h"

#include "BatchAsset.h"
#include "ScannerBase.h"
#include "FilterBase.h"
#include "ProcessorBase.h"
#include "ConditionBase.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchPipelineView"

// ── 颜色常量 ─────────────────────────────────────────────────────────────────
const FLinearColor SBatchPipelineView::ScannerColor   = FLinearColor(0.20f, 0.50f, 0.90f);
const FLinearColor SBatchPipelineView::FilterColor    = FLinearColor(0.90f, 0.55f, 0.15f);
const FLinearColor SBatchPipelineView::ProcessorColor = FLinearColor(0.20f, 0.72f, 0.45f);

// ─────────────────────────────────────────────────────────────────────────────
// Construct
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::Construct(const FArguments& InArgs)
{
    AssetPtr           = InArgs._Asset;
    OnSelectionChanged = InArgs._OnSelectionChanged;

    ChildSlot
    [
        SNew(SScrollBox)
        .Orientation(Orient_Vertical)
        + SScrollBox::Slot()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot().FillWidth(1.f)
            [
                BuildLane(LOCTEXT("ScannerLane", "扫描器"), ScannerColor,
                    ScannerEntries, ScannerCardsBox, UScannerBase::StaticClass(),
                    [this](UClass* C){ OnScannerAdd(C); },
                    [this](int32 I) { OnScannerRemove(I); })
            ]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Top).Padding(0.f, 36.f)
            [ BuildArrow() ]

            + SHorizontalBox::Slot().FillWidth(1.f)
            [
                BuildLane(LOCTEXT("FilterLane", "过滤器"), FilterColor,
                    FilterEntries, FilterCardsBox, UFilterBase::StaticClass(),
                    [this](UClass* C){ OnFilterAdd(C); },
                    [this](int32 I) { OnFilterRemove(I); })
            ]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Top).Padding(0.f, 36.f)
            [ BuildArrow() ]

            + SHorizontalBox::Slot().FillWidth(1.f)
            [
                BuildLane(LOCTEXT("ProcessorLane", "处理器"), ProcessorColor,
                    ProcessorEntries, ProcessorCardsBox, UProcessorBase::StaticClass(),
                    [this](UClass* C){ OnProcessorAdd(C); },
                    [this](int32 I) { OnProcessorRemove(I); })
            ]
        ]
    ];

    Refresh();
}

// ─────────────────────────────────────────────────────────────────────────────
// Refresh
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::Refresh()
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;

    auto ToObjArray = [](const auto& Src, TArray<UObject*>& Dst)
    {
        Dst.Reset(Src.Num());
        for (auto* Item : Src) Dst.Add(Item);
    };

    TArray<UObject*> ScanObjs, FilterObjs, ProcObjs;
    ToObjArray(Asset->GetScanners(),   ScanObjs);
    ToObjArray(Asset->GetFilters(),    FilterObjs);
    ToObjArray(Asset->GetProcessors(), ProcObjs);

    PopulateEntries(ScannerEntries,   ScanObjs);
    PopulateEntries(FilterEntries,    FilterObjs);
    PopulateEntries(ProcessorEntries, ProcObjs);

    if (ScannerCardsBox)
        RebuildCards(*ScannerCardsBox,   ScannerEntries,   ScannerColor,   [this](int32 I){ OnScannerRemove(I); });
    if (FilterCardsBox)
        RebuildCards(*FilterCardsBox,    FilterEntries,    FilterColor,    [this](int32 I){ OnFilterRemove(I); });
    if (ProcessorCardsBox)
        RebuildCards(*ProcessorCardsBox, ProcessorEntries, ProcessorColor, [this](int32 I){ OnProcessorRemove(I); });
}

void SBatchPipelineView::PopulateEntries(TArray<FEntryPtr>& Out, const TArray<UObject*>& Components) const
{
    Out.Reset(Components.Num());
    for (UObject* Comp : Components)
    {
        if (!Comp) continue;
        auto Entry = MakeShared<FPipelineEntry>();
        Entry->Component = Comp;

        const FString DN = Comp->GetClass()->GetMetaData(TEXT("DisplayName"));
        Entry->DisplayName = DN.IsEmpty()
            ? FText::FromString(Comp->GetClass()->GetName())
            : FText::FromString(DN);

        TArray<FText> Errors;
        if (auto* S = Cast<UScannerBase>(Comp))   S->ValidateConfig(Errors);
        if (auto* F = Cast<UFilterBase>(Comp))    F->ValidateConfig(Errors);
        if (auto* P = Cast<UProcessorBase>(Comp)) P->ValidateConfig(Errors);
        Entry->bHasError = !Errors.IsEmpty();
        if (Entry->bHasError) Entry->ErrorText = Errors[0];

        // 通过反射读取备注属性（各基类均有 Remark FStrProperty）
        if (const FStrProperty* RemarkProp = FindFProperty<FStrProperty>(Comp->GetClass(), TEXT("Remark")))
            Entry->Remark = RemarkProp->GetPropertyValue_InContainer(Comp);

        PopulateSubGroups(*Entry);
        Out.Add(Entry);
    }
}

void SBatchPipelineView::PopulateSubGroups(FPipelineEntry& Entry) const
{
    Entry.SubGroups.Reset();
    UObject* Comp = Entry.Component;
    if (!Comp) return;

    for (TFieldIterator<FArrayProperty> It(Comp->GetClass()); It; ++It)
    {
        FArrayProperty* ArrayProp = *It;
        FObjectProperty* InnerProp = CastField<FObjectProperty>(ArrayProp->Inner);
        if (!InnerProp) continue;

        // 仅扫描子处理器数组；条件数组在属性面板中配置，不在流水线内联展示
        UClass* ElemBase = nullptr;
        UClass* ElemClass = InnerProp->PropertyClass;
        if (ElemClass->IsChildOf(UProcessorBase::StaticClass())) ElemBase = UProcessorBase::StaticClass();
        else continue;

        FSubGroupEntry Group;
        Group.ArrayProp   = ArrayProp;
        Group.ElementBase = ElemBase;
        const FString PropDN = ArrayProp->GetMetaData(TEXT("DisplayName"));
        Group.DisplayName = PropDN.IsEmpty()
            ? FText::FromName(ArrayProp->GetFName())
            : FText::FromString(PropDN);

        // 读取当前条目
        FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(Comp));
        for (int32 i = 0; i < ArrayHelper.Num(); ++i)
        {
            UObject* Item = InnerProp->GetObjectPropertyValue(ArrayHelper.GetRawPtr(i));
            if (!Item) continue;
            auto SubEntry = MakeShared<FPipelineEntry>();
            SubEntry->Component = Item;
            const FString ItemDN = Item->GetClass()->GetMetaData(TEXT("DisplayName"));
            SubEntry->DisplayName = ItemDN.IsEmpty()
                ? FText::FromString(Item->GetClass()->GetName())
                : FText::FromString(ItemDN);
            if (const FStrProperty* RP = FindFProperty<FStrProperty>(Item->GetClass(), TEXT("Remark")))
                SubEntry->Remark = RP->GetPropertyValue_InContainer(Item);
            PopulateSubGroups(*SubEntry); // 递归扫描子条目自身的子组
            Group.Items.Add(SubEntry);
        }
        Entry.SubGroups.Add(MoveTemp(Group));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 泳道构建
// ─────────────────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SBatchPipelineView::BuildLane(
    const FText& Title, const FLinearColor& HeaderColor,
    TArray<FEntryPtr>& Entries, TSharedPtr<SVerticalBox>& OutCardsBox,
    UClass* BaseClass, TFunction<void(UClass*)> AddFn, TFunction<void(int32)> RemoveFn)
{
    TSharedRef<SVerticalBox> CardsBox = SNew(SVerticalBox);
    OutCardsBox = CardsBox;

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
        .Padding(0.f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FSlateColor(HeaderColor * 0.6f))
                .Padding(FMargin(8.f, 6.f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
                    [
                        SNew(STextBlock).Text(Title)
                        .Font(FAppStyle::Get().GetFontStyle("BoldFont"))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                    [ BuildAddMenu(BaseClass, AddFn) ]
                ]
            ]
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FSlateColor(HeaderColor))
                .Padding(0.f, 2.f)
            ]
            + SVerticalBox::Slot().FillHeight(1.f).Padding(6.f, 4.f)
            [ CardsBox ]
        ];
}

TSharedRef<SWidget> SBatchPipelineView::BuildArrow() const
{
    return SNew(SBox).WidthOverride(40.f).HeightOverride(32.f).VAlign(VAlign_Center).HAlign(HAlign_Center)
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("──▶")))
        .ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
    ];
}

TSharedRef<SWidget> SBatchPipelineView::BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn) const
{
    TArray<UClass*> SubClasses;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* C = *It;
        if (C->IsChildOf(BaseClass) && C != BaseClass
            && !C->HasAnyClassFlags(CLASS_Abstract)
            && !C->HasAnyClassFlags(CLASS_Deprecated))
            SubClasses.Add(C);
    }
    SubClasses.Sort([](const UClass& A, const UClass& B){ return A.GetName() < B.GetName(); });

    FMenuBuilder MB(true, nullptr);
    for (UClass* C : SubClasses)
    {
        const FString DN = C->GetMetaData(TEXT("DisplayName"));
        const FText Label = DN.IsEmpty() ? FText::FromString(C->GetName()) : FText::FromString(DN);
        MB.AddMenuEntry(Label, FText::FromString(C->GetName()), FSlateIcon(),
            FUIAction(FExecuteAction::CreateLambda([AddFn, C](){ AddFn(C); })));
    }
    return SNew(SComboButton)
        .ButtonStyle(FAppStyle::Get(), "NoBorder")
        .ForegroundColor(FSlateColor(FLinearColor::White))
        .ContentPadding(FMargin(4.f, 2.f))
        .ButtonContent()[ SNew(STextBlock).Text(FText::FromString(TEXT("＋"))).ColorAndOpacity(FSlateColor(FLinearColor::White)) ]
        .MenuContent()[ MB.MakeWidget() ];
}

// ─────────────────────────────────────────────────────────────────────────────
// 卡片渲染
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::RebuildCards(SVerticalBox& CardsBox,
                                       const TArray<FEntryPtr>& Entries,
                                       const FLinearColor& AccentColor,
                                       TFunction<void(int32)> RemoveFn)
{
    CardsBox.ClearChildren();
    for (int32 i = 0; i < Entries.Num(); ++i)
    {
        CardsBox.AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
        [ BuildCard(Entries[i], i, AccentColor, RemoveFn) ];
    }
    if (Entries.IsEmpty())
    {
        CardsBox.AddSlot().AutoHeight().Padding(4.f, 8.f)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("EmptyLane", "暂无组件"))
            .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            .Justification(ETextJustify::Center)
        ];
    }
}

TSharedRef<SWidget> SBatchPipelineView::BuildCard(FEntryPtr Entry, int32 Index,
                                                    const FLinearColor& AccentColor,
                                                    TFunction<void(int32)> RemoveFn)
{
    const bool bHasError = Entry->bHasError;
    const bool bSelected = (Entry->Component == SelectedComponent.Get());
    const bool bHasSubs  = !Entry->SubGroups.IsEmpty();
    const bool bExpanded = bHasSubs && !CollapsedComponents.Contains(Entry->Component);

    const FLinearColor BarColor  = bHasError ? FLinearColor(0.9f, 0.55f, 0.05f) : AccentColor;
    const FLinearColor BgTint    = bSelected ? FLinearColor(BarColor.R, BarColor.G, BarColor.B, 0.18f) : FLinearColor::Transparent;
    const float        BarWidth  = bSelected ? 5.f : 3.f;
    const FLinearColor BarFinal  = bSelected ? BarColor * 1.4f : BarColor * 0.85f;

    TWeakPtr<FPipelineEntry> WeakEntry = Entry;

    // 主行内容
    TSharedRef<SWidget> MainRow =
        SNew(SHorizontalBox)

        // 左侧彩色竖条
        + SHorizontalBox::Slot().AutoWidth()
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FSlateColor(BarFinal))
            .Padding(BarWidth, 0.f)
        ]

        // 内容区
        + SHorizontalBox::Slot().FillWidth(1.f).Padding(6.f, 4.f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(SHorizontalBox)
                // 状态图标
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
                [
                    SNew(STextBlock)
                    .Text(bHasError ? FText::FromString(TEXT("⚠")) : FText::FromString(TEXT("●")))
                    .ColorAndOpacity(FSlateColor(bHasError ? FLinearColor(1.f,0.7f,0.f) : BarFinal))
                ]
                // 组件名
                + SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(Entry->DisplayName)
                    .Font(bSelected ? FAppStyle::Get().GetFontStyle("BoldFont") : FAppStyle::Get().GetFontStyle("SmallFont"))
                    .ColorAndOpacity(FSlateColor(bSelected ? FLinearColor::White : FSlateColor::UseForeground()))
                    .ToolTipText(bHasError ? Entry->ErrorText : FText::GetEmpty())
                ]
            ]
            // 备注（有内容时显示，灰色斜体）
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(STextBlock)
                .Visibility(!Entry->Remark.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed)
                .Text(FText::FromString(Entry->Remark))
                .Font(FCoreStyle::GetDefaultFontStyle("Italic", 7))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.55f, 0.55f)))
                .AutoWrapText(true)
            ]
            // 校验错误
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(STextBlock)
                .Visibility(bHasError ? EVisibility::Visible : EVisibility::Collapsed)
                .Text(Entry->ErrorText)
                .Font(FCoreStyle::GetDefaultFontStyle("Italic", 7))
                .ColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.7f, 0.f)))
                .AutoWrapText(true)
            ]
        ]

        // 展开/折叠切换（仅有子组时显示）
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 2.f, 0.f)
        [
            SNew(SButton)
            .Visibility(bHasSubs ? EVisibility::Visible : EVisibility::Collapsed)
            .ButtonStyle(FAppStyle::Get(), "NoBorder")
            .ToolTipText(bExpanded ? LOCTEXT("Collapse", "折叠子组件") : LOCTEXT("Expand", "展开子组件"))
            .OnClicked_Lambda([this, WeakEntry]() -> FReply
            {
                if (auto Pinned = WeakEntry.Pin())
                {
                    if (CollapsedComponents.Contains(Pinned->Component))
                        CollapsedComponents.Remove(Pinned->Component);
                    else
                        CollapsedComponents.Add(Pinned->Component);
                    if (ScannerCardsBox)
                        RebuildCards(*ScannerCardsBox, ScannerEntries, ScannerColor, [this](int32 I){ OnScannerRemove(I); });
                    if (FilterCardsBox)
                        RebuildCards(*FilterCardsBox, FilterEntries, FilterColor, [this](int32 I){ OnFilterRemove(I); });
                    if (ProcessorCardsBox)
                        RebuildCards(*ProcessorCardsBox, ProcessorEntries, ProcessorColor, [this](int32 I){ OnProcessorRemove(I); });
                }
                return FReply::Handled();
            })
            [
                SNew(SImage)
                .Image(bExpanded
                    ? FCoreStyle::Get().GetBrush("TreeArrow_Expanded")
                    : FCoreStyle::Get().GetBrush("TreeArrow_Collapsed"))
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
        ]

        // 删除按钮
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
        [
            SNew(SButton)
            .ButtonStyle(FAppStyle::Get(), "NoBorder")
            .ToolTipText(LOCTEXT("RemoveCard", "移除此组件"))
            .OnClicked_Lambda([RemoveFn, Index]() -> FReply { RemoveFn(Index); return FReply::Handled(); })
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("✕")))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
        ];

    // 组装：高亮仅作用于标题行，子组区域无高亮
    return SNew(SVerticalBox)
        // 标题行（含选中高亮描边 + 背景 tint）
        + SVerticalBox::Slot().AutoHeight()
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FSlateColor(bSelected
                ? FLinearColor(BarColor.R, BarColor.G, BarColor.B, 0.6f)
                : FLinearColor::Transparent))
            .Padding(bSelected ? 1.f : 0.f)
            .Cursor(EMouseCursor::Hand)
            .OnMouseButtonDown_Lambda([this, WeakEntry](const FGeometry&, const FPointerEvent& Event) -> FReply
            {
                if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
                {
                    if (auto Pinned = WeakEntry.Pin()) SelectComponent(Pinned->Component);
                    return FReply::Handled();
                }
                return FReply::Unhandled();
            })
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                .BorderBackgroundColor(FSlateColor(BgTint))
                .Padding(0.f)
                [ MainRow ]
            ]
        ]
        // 子组区域：accent 色导引线 + 轻微背景加深
        + SVerticalBox::Slot().AutoHeight()
        [
            SNew(SBox)
            .Visibility(bExpanded ? EVisibility::Visible : EVisibility::Collapsed)
            [
                SNew(SHorizontalBox)
                // 左侧导引线（accent 色，贯通子组高度）
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FSlateColor(FLinearColor(BarColor.R, BarColor.G, BarColor.B, 0.45f)))
                    .Padding(FMargin(2.f, 0.f))
                ]
                // 子组内容（背景轻微加深）
                + SHorizontalBox::Slot().FillWidth(1.f)
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.14f)))
                    .Padding(0.f)
                    [ BuildSubGroups(Entry) ]
                ]
            ]
        ];
}

// ─────────────────────────────────────────────────────────────────────────────
// 子组区域构建
// ─────────────────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SBatchPipelineView::BuildSubGroups(FEntryPtr Entry)
{
    TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);

    // 分隔线
    Box->AddSlot().AutoHeight()
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FSlateColor(FLinearColor(1.f,1.f,1.f,0.08f)))
        .Padding(0.f, 1.f)
    ];

    for (FSubGroupEntry& Group : Entry->SubGroups)
    {
        Box->AddSlot().AutoHeight()
        [ BuildSubGroup(Entry, Group) ];
    }
    return Box;
}

TSharedRef<SWidget> SBatchPipelineView::BuildSubGroup(FEntryPtr ParentEntry, FSubGroupEntry& Group)
{
    UClass* ElemBase = Group.ElementBase;
    FArrayProperty* ArrayProp = Group.ArrayProp;
    TWeakObjectPtr<UObject> WeakParent = ParentEntry->Component;

    // 枚举子类
    TArray<UClass*> SubClasses;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* C = *It;
        if (C->IsChildOf(ElemBase) && C != ElemBase
            && !C->HasAnyClassFlags(CLASS_Abstract)
            && !C->HasAnyClassFlags(CLASS_Deprecated))
            SubClasses.Add(C);
    }
    SubClasses.Sort([](const UClass& A, const UClass& B){ return A.GetName() < B.GetName(); });

    FMenuBuilder MB(true, nullptr);
    for (UClass* C : SubClasses)
    {
        const FString DN = C->GetMetaData(TEXT("DisplayName"));
        const FText Label = DN.IsEmpty() ? FText::FromString(C->GetName()) : FText::FromString(DN);
        MB.AddMenuEntry(Label, FText::FromString(C->GetName()), FSlateIcon(),
            FUIAction(FExecuteAction::CreateLambda([this, WeakParent, ArrayProp, C]()
            {
                if (auto* P = WeakParent.Get()) OnSubComponentAdd(P, ArrayProp, C);
            })));
    }

    TSharedRef<SVerticalBox> ItemsBox = SNew(SVerticalBox);

    // 子条目列表
    for (int32 i = 0; i < Group.Items.Num(); ++i)
    {
        ItemsBox->AddSlot().AutoHeight().Padding(0.f, 1.f)
        [ BuildSubItem(ParentEntry, Group, Group.Items[i], i) ];
    }
    if (Group.Items.IsEmpty())
    {
        ItemsBox->AddSlot().AutoHeight().Padding(4.f, 2.f)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("EmptySubGroup", "暂无"))
            .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 7))
        ];
    }

    // 添加行（底部，无标题头）
    ItemsBox->AddSlot().AutoHeight().Padding(FMargin(22.f, 2.f, 4.f, 2.f))
    [
        SNew(SComboButton)
        .ButtonStyle(FAppStyle::Get(), "NoBorder")
        .ContentPadding(FMargin(2.f, 1.f))
        .ButtonContent()
        [
            SNew(STextBlock).Text(LOCTEXT("AddSubProc", "＋ 添加"))
            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
            .ColorAndOpacity(FSlateColor::UseSubduedForeground())
        ]
        .MenuContent()[ MB.MakeWidget() ]
    ];

    return SNew(SBox).Padding(FMargin(0.f, 2.f, 0.f, 0.f))
    [ ItemsBox ];
}

TSharedRef<SWidget> SBatchPipelineView::BuildSubItem(FEntryPtr ParentEntry, FSubGroupEntry& Group,
                                                       FEntryPtr SubEntry, int32 SubIndex)
{
    const bool bSubSelected = (SubEntry->Component == SelectedComponent.Get());
    const bool bSubHasSubs  = !SubEntry->SubGroups.IsEmpty();
    const bool bSubExpanded = bSubHasSubs && !CollapsedComponents.Contains(SubEntry->Component);
    FArrayProperty* ArrayProp = Group.ArrayProp;
    TWeakObjectPtr<UObject> WeakParent = ParentEntry->Component;
    TWeakPtr<FPipelineEntry> WeakSub   = SubEntry;
    const int32 CapturedIdx = SubIndex;

    // 主行
    TSharedRef<SWidget> Row =
        SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FSlateColor(bSubSelected
            ? FLinearColor(0.3f,0.3f,0.3f,0.4f) : FLinearColor::Transparent))
        .Padding(0.f)
        .Cursor(EMouseCursor::Hand)
        .OnMouseButtonDown_Lambda([this, WeakSub](const FGeometry&, const FPointerEvent& Event) -> FReply
        {
            if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
            {
                if (auto P = WeakSub.Pin()) SelectComponent(P->Component);
                return FReply::Handled();
            }
            return FReply::Unhandled();
        })
        [
            SNew(SHorizontalBox)
            // 树形缩进
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("  ├─")))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 7))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.4f,0.4f,0.4f)))
            ]
            // 展开/折叠（仅有子组时显示）
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
            [
                SNew(SButton)
                .Visibility(bSubHasSubs ? EVisibility::Visible : EVisibility::Collapsed)
                .ButtonStyle(FAppStyle::Get(), "NoBorder")
                .ToolTipText(bSubExpanded ? LOCTEXT("SubCollapse","折叠") : LOCTEXT("SubExpand","展开"))
                .OnClicked_Lambda([this, WeakSub]() -> FReply
                {
                    if (auto P = WeakSub.Pin())
                    {
                        if (CollapsedComponents.Contains(P->Component))
                            CollapsedComponents.Remove(P->Component);
                        else
                            CollapsedComponents.Add(P->Component);
                        if (ScannerCardsBox)   RebuildCards(*ScannerCardsBox,   ScannerEntries,   ScannerColor,   [this](int32 I){ OnScannerRemove(I); });
                        if (FilterCardsBox)    RebuildCards(*FilterCardsBox,    FilterEntries,    FilterColor,    [this](int32 I){ OnFilterRemove(I); });
                        if (ProcessorCardsBox) RebuildCards(*ProcessorCardsBox, ProcessorEntries, ProcessorColor, [this](int32 I){ OnProcessorRemove(I); });
                    }
                    return FReply::Handled();
                })
                [
                    SNew(SImage)
                    .Image(bSubExpanded
                        ? FCoreStyle::Get().GetBrush("TreeArrow_Expanded")
                        : FCoreStyle::Get().GetBrush("TreeArrow_Collapsed"))
                    .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                ]
            ]
            // 状态 + 名称
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("●")))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 7))
                .ColorAndOpacity(FSlateColor(bSubSelected
                    ? FLinearColor(0.9f,0.9f,0.9f) : FLinearColor(0.5f,0.5f,0.5f)))
            ]
            + SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center).Padding(2.f, 2.f)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight()
                [
                    SNew(STextBlock).Text(SubEntry->DisplayName)
                    .Font(bSubSelected ? FAppStyle::Get().GetFontStyle("SmallFont") : FCoreStyle::GetDefaultFontStyle("Regular", 8))
                    .ColorAndOpacity(FSlateColor(bSubSelected ? FLinearColor::White : FSlateColor::UseSubduedForeground()))
                ]
                + SVerticalBox::Slot().AutoHeight()
                [
                    SNew(STextBlock)
                    .Visibility(!SubEntry->Remark.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed)
                    .Text(FText::FromString(SubEntry->Remark))
                    .Font(FCoreStyle::GetDefaultFontStyle("Italic", 6))
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
                    .AutoWrapText(true)
                ]
            ]
            // 删除
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), "NoBorder")
                .ToolTipText(LOCTEXT("RemoveSubItem", "移除"))
                .OnClicked_Lambda([this, WeakParent, ArrayProp, CapturedIdx]() -> FReply
                {
                    if (auto* P = WeakParent.Get()) OnSubComponentRemove(P, ArrayProp, CapturedIdx);
                    return FReply::Handled();
                })
                [
                    SNew(STextBlock).Text(FText::FromString(TEXT("✕")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 7))
                    .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                ]
            ]
        ];

    // 若有子组且已展开，递归渲染（利用 BuildSubGroups，左侧额外缩进区分层级）
    if (!bSubHasSubs) return Row;

    return SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [ Row ]
        + SVerticalBox::Slot().AutoHeight()
        [
            SNew(SBox)
            .Visibility(bSubExpanded ? EVisibility::Visible : EVisibility::Collapsed)
            .Padding(FMargin(20.f, 0.f, 0.f, 0.f))
            [
                SNew(SHorizontalBox)
                // 灰色导引线（第二层级）
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f, 0.45f)))
                    .Padding(FMargin(2.f, 0.f))
                ]
                // 子子组内容（背景再加深一级）
                + SHorizontalBox::Slot().FillWidth(1.f)
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.12f)))
                    .Padding(0.f)
                    [ BuildSubGroups(SubEntry) ]
                ]
            ]
        ];
}

// ─────────────────────────────────────────────────────────────────────────────
// 选中
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::SelectComponent(UObject* Component)
{
    if (SelectedComponent.Get() == Component) return; // 已选中，不取消
    SelectedComponent = Component;
    OnSelectionChanged.ExecuteIfBound(Component);

    if (ScannerCardsBox)
        RebuildCards(*ScannerCardsBox,   ScannerEntries,   ScannerColor,   [this](int32 I){ OnScannerRemove(I); });
    if (FilterCardsBox)
        RebuildCards(*FilterCardsBox,    FilterEntries,    FilterColor,    [this](int32 I){ OnFilterRemove(I); });
    if (ProcessorCardsBox)
        RebuildCards(*ProcessorCardsBox, ProcessorEntries, ProcessorColor, [this](int32 I){ OnProcessorRemove(I); });
}

// ─────────────────────────────────────────────────────────────────────────────
// 添加 / 删除（顶层）
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::OnScannerAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;
    FScopedTransaction Tx(LOCTEXT("AddScanner", "添加扫描器"));
    Asset->Modify();
    Asset->AddScanner(NewObject<UScannerBase>(Asset, Class, NAME_None, RF_Transactional));
    Asset->MarkPackageDirty();
    Refresh();
}
void SBatchPipelineView::OnFilterAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;
    FScopedTransaction Tx(LOCTEXT("AddFilter", "添加过滤器"));
    Asset->Modify();
    Asset->AddFilter(NewObject<UFilterBase>(Asset, Class, NAME_None, RF_Transactional));
    Asset->MarkPackageDirty();
    Refresh();
}
void SBatchPipelineView::OnProcessorAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;
    FScopedTransaction Tx(LOCTEXT("AddProcessor", "添加处理器"));
    Asset->Modify();
    Asset->AddProcessor(NewObject<UProcessorBase>(Asset, Class, NAME_None, RF_Transactional));
    Asset->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnScannerRemove(int32 Index)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;
    if (Asset->GetScanners().IsValidIndex(Index) && SelectedComponent.Get() == Asset->GetScanners()[Index])
    { SelectedComponent = nullptr; OnSelectionChanged.ExecuteIfBound(nullptr); }
    FScopedTransaction Tx(LOCTEXT("RemoveScanner", "删除扫描器"));
    Asset->Modify(); Asset->RemoveScanner(Index); Asset->MarkPackageDirty(); Refresh();
}
void SBatchPipelineView::OnFilterRemove(int32 Index)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;
    if (Asset->GetFilters().IsValidIndex(Index) && SelectedComponent.Get() == Asset->GetFilters()[Index])
    { SelectedComponent = nullptr; OnSelectionChanged.ExecuteIfBound(nullptr); }
    FScopedTransaction Tx(LOCTEXT("RemoveFilter", "删除过滤器"));
    Asset->Modify(); Asset->RemoveFilter(Index); Asset->MarkPackageDirty(); Refresh();
}
void SBatchPipelineView::OnProcessorRemove(int32 Index)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;
    if (Asset->GetProcessors().IsValidIndex(Index) && SelectedComponent.Get() == Asset->GetProcessors()[Index])
    { SelectedComponent = nullptr; OnSelectionChanged.ExecuteIfBound(nullptr); }
    FScopedTransaction Tx(LOCTEXT("RemoveProcessor", "删除处理器"));
    Asset->Modify(); Asset->RemoveProcessor(Index); Asset->MarkPackageDirty(); Refresh();
}

// ─────────────────────────────────────────────────────────────────────────────
// 添加 / 删除（子组件，通过反射操作）
// ─────────────────────────────────────────────────────────────────────────────

void SBatchPipelineView::OnSubComponentAdd(UObject* Parent, FArrayProperty* ArrayProp, UClass* Class)
{
    if (!Parent || !ArrayProp || !Class) return;
    FScopedTransaction Tx(LOCTEXT("AddSubComp", "添加子组件"));
    Parent->Modify();

    FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(Parent));
    const int32 NewIdx = ArrayHelper.AddValue();
    FObjectProperty* InnerProp = CastField<FObjectProperty>(ArrayProp->Inner);
    UObject* NewObj = NewObject<UObject>(Parent, Class, NAME_None, RF_Transactional);
    InnerProp->SetObjectPropertyValue(ArrayHelper.GetRawPtr(NewIdx), NewObj);

    Parent->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnSubComponentRemove(UObject* Parent, FArrayProperty* ArrayProp, int32 Index)
{
    if (!Parent || !ArrayProp) return;

    // 若删除的子组件正被选中，清除选中
    {
        FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(Parent));
        if (ArrayHelper.IsValidIndex(Index))
        {
            FObjectProperty* InnerProp = CastField<FObjectProperty>(ArrayProp->Inner);
            UObject* Item = InnerProp->GetObjectPropertyValue(ArrayHelper.GetRawPtr(Index));
            if (SelectedComponent.Get() == Item)
            { SelectedComponent = nullptr; OnSelectionChanged.ExecuteIfBound(nullptr); }
        }
    }

    FScopedTransaction Tx(LOCTEXT("RemoveSubComp", "删除子组件"));
    Parent->Modify();
    FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(Parent));
    ArrayHelper.RemoveValues(Index);
    Parent->MarkPackageDirty();
    Refresh();
}

#undef LOCTEXT_NAMESPACE
