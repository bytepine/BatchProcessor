// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchPipelineView.h"

#include "BatchAsset.h"
#include "ScannerBase.h"
#include "FilterBase.h"
#include "ProcessorBase.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSeparator.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchPipelineView"

// ── 类型颜色 ─────────────────────────────────────────────────────────────────
const FLinearColor SBatchPipelineView::ScannerColor   = FLinearColor(0.20f, 0.50f, 0.90f); // 蓝
const FLinearColor SBatchPipelineView::FilterColor    = FLinearColor(0.90f, 0.55f, 0.15f); // 橙
const FLinearColor SBatchPipelineView::ProcessorColor = FLinearColor(0.20f, 0.72f, 0.45f); // 绿

// ── Construct ────────────────────────────────────────────────────────────────

void SBatchPipelineView::Construct(const FArguments& InArgs)
{
    AssetPtr          = InArgs._Asset;
    OnSelectionChanged = InArgs._OnSelectionChanged;

    // 三泳道 + 两箭头，横向铺开，外层 ScrollBox 允许窄布局时水平滚动
    ChildSlot
    [
        SNew(SScrollBox)
        .Orientation(Orient_Vertical)
        + SScrollBox::Slot()
        [
            SNew(SHorizontalBox)

            // ── 扫描器泳道 ────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                BuildLane(
                    LOCTEXT("ScannerLane", "SCANNERS"),
                    ScannerColor,
                    ScannerEntries,
                    ScannerCardsBox,
                    UScannerBase::StaticClass(),
                    [this](UClass* C){ OnScannerAdd(C); },
                    [this](int32 I) { OnScannerRemove(I); })
            ]

            // ── 箭头 ─────────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Top)
            .Padding(0.f, 36.f) // 与标题头垂直对齐
            [
                BuildArrow()
            ]

            // ── 过滤器泳道 ────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                BuildLane(
                    LOCTEXT("FilterLane", "FILTERS"),
                    FilterColor,
                    FilterEntries,
                    FilterCardsBox,
                    UFilterBase::StaticClass(),
                    [this](UClass* C){ OnFilterAdd(C); },
                    [this](int32 I) { OnFilterRemove(I); })
            ]

            // ── 箭头 ─────────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Top)
            .Padding(0.f, 36.f)
            [
                BuildArrow()
            ]

            // ── 处理器泳道 ────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                BuildLane(
                    LOCTEXT("ProcessorLane", "PROCESSORS"),
                    ProcessorColor,
                    ProcessorEntries,
                    ProcessorCardsBox,
                    UProcessorBase::StaticClass(),
                    [this](UClass* C){ OnProcessorAdd(C); },
                    [this](int32 I) { OnProcessorRemove(I); })
            ]
        ]
    ];

    Refresh();
}

// ── Refresh ──────────────────────────────────────────────────────────────────

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

    if (ScannerCardsBox)   RebuildCards(*ScannerCardsBox,   ScannerEntries,   [this](int32 I){ OnScannerRemove(I); });
    if (FilterCardsBox)    RebuildCards(*FilterCardsBox,    FilterEntries,    [this](int32 I){ OnFilterRemove(I); });
    if (ProcessorCardsBox) RebuildCards(*ProcessorCardsBox, ProcessorEntries, [this](int32 I){ OnProcessorRemove(I); });
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
        if (auto* S = Cast<UScannerBase>(Comp))    S->ValidateConfig(Errors);
        if (auto* F = Cast<UFilterBase>(Comp))     F->ValidateConfig(Errors);
        if (auto* P = Cast<UProcessorBase>(Comp))  P->ValidateConfig(Errors);

        Entry->bHasError = !Errors.IsEmpty();
        if (Entry->bHasError) Entry->ErrorText = Errors[0];

        Out.Add(Entry);
    }
}

// ── 泳道构建 ─────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SBatchPipelineView::BuildLane(
    const FText& Title,
    const FLinearColor& HeaderColor,
    TArray<FEntryPtr>& Entries,
    TSharedPtr<SVerticalBox>& OutCardsBox,
    UClass* BaseClass,
    TFunction<void(UClass*)> AddFn,
    TFunction<void(int32)> RemoveFn)
{
    TSharedRef<SVerticalBox> CardsBox = SNew(SVerticalBox);
    OutCardsBox = CardsBox;

    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
        .Padding(0.f)
        [
            SNew(SVerticalBox)

            // ── 标题头（彩色背景）───────────────────────────────────────────
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FSlateColor(HeaderColor * 0.6f))
                .Padding(FMargin(8.f, 6.f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(Title)
                        .Font(FAppStyle::GetFontStyle("BoldFont"))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        BuildAddMenu(BaseClass, AddFn)
                    ]
                ]
            ]

            // ── 分隔线 ───────────────────────────────────────────────────────
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FSlateColor(HeaderColor))
                .Padding(0.f, 2.f)
            ]

            // ── 卡片区 ───────────────────────────────────────────────────────
            + SVerticalBox::Slot()
            .FillHeight(1.f)
            .Padding(6.f, 4.f)
            [
                CardsBox
            ]
        ];
}

TSharedRef<SWidget> SBatchPipelineView::BuildArrow() const
{
    return SNew(SBox)
        .WidthOverride(40.f)
        .HeightOverride(32.f)
        .VAlign(VAlign_Center)
        .HAlign(HAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("──▶")))
            .ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
        ];
}

TSharedRef<SWidget> SBatchPipelineView::BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn) const
{
    // 枚举所有已注册的非 Abstract 具体子类
    TArray<UClass*> SubClasses;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* C = *It;
        if (C->IsChildOf(BaseClass)
            && C != BaseClass
            && !C->HasAnyClassFlags(CLASS_Abstract)
            && !C->HasAnyClassFlags(CLASS_Deprecated))
        {
            SubClasses.Add(C);
        }
    }
    SubClasses.Sort([](const UClass& A, const UClass& B){ return A.GetName() < B.GetName(); });

    FMenuBuilder MenuBuilder(true, nullptr);
    for (UClass* C : SubClasses)
    {
        const FString DN = C->GetMetaData(TEXT("DisplayName"));
        const FText Label = DN.IsEmpty() ? FText::FromString(C->GetName()) : FText::FromString(DN);
        MenuBuilder.AddMenuEntry(
            Label, FText::FromString(C->GetName()), FSlateIcon(),
            FUIAction(FExecuteAction::CreateLambda([AddFn, C](){ AddFn(C); })));
    }

    return SNew(SComboButton)
        .ButtonStyle(FAppStyle::Get(), "NoBorder")
        .ForegroundColor(FSlateColor(FLinearColor::White))
        .ContentPadding(FMargin(4.f, 2.f))
        .ButtonContent()
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("＋")))
            .ColorAndOpacity(FSlateColor(FLinearColor::White))
        ]
        .MenuContent()
        [
            MenuBuilder.MakeWidget()
        ];
}

// ── 卡片渲染 ─────────────────────────────────────────────────────────────────

void SBatchPipelineView::RebuildCards(SVerticalBox& CardsBox,
                                      const TArray<FEntryPtr>& Entries,
                                      TFunction<void(int32)> RemoveFn)
{
    CardsBox.ClearChildren();

    // 确定这是哪类组件来取颜色
    FLinearColor AccentColor = FLinearColor::White;
    if (!Entries.IsEmpty() && Entries[0]->Component)
    {
        if (Entries[0]->Component->IsA<UScannerBase>())   AccentColor = ScannerColor;
        else if (Entries[0]->Component->IsA<UFilterBase>())    AccentColor = FilterColor;
        else if (Entries[0]->Component->IsA<UProcessorBase>()) AccentColor = ProcessorColor;
    }

    for (int32 i = 0; i < Entries.Num(); ++i)
    {
        const FEntryPtr& Entry = Entries[i];
        const int32 CapturedIndex = i;

        CardsBox.AddSlot()
        .AutoHeight()
        .Padding(0.f, 0.f, 0.f, 4.f)
        [
            BuildCard(Entry, CapturedIndex, AccentColor, RemoveFn)
        ];
    }

    // 空状态提示
    if (Entries.IsEmpty())
    {
        CardsBox.AddSlot()
        .AutoHeight()
        .Padding(4.f, 8.f)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("EmptyLane", "暂无组件"))
            .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            .Justification(ETextJustify::Center)
        ];
    }
}

TSharedRef<SWidget> SBatchPipelineView::BuildCard(FEntryPtr Entry,
                                                   int32 Index,
                                                   const FLinearColor& AccentColor,
                                                   TFunction<void(int32)> RemoveFn)
{
    const bool bHasError = Entry->bHasError;
    const FLinearColor CardAccent = bHasError ? FLinearColor(0.9f, 0.55f, 0.05f) : AccentColor;
    TWeakPtr<FPipelineEntry> WeakEntry = Entry;

    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(0.f)
        .Cursor(EMouseCursor::Hand)
        .OnMouseButtonDown_Lambda([this, WeakEntry](const FGeometry&, const FPointerEvent& Event) -> FReply
        {
            if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
            {
                if (auto Pinned = WeakEntry.Pin()) SelectEntry(Pinned);
                return FReply::Handled();
            }
            return FReply::Unhandled();
        })
        [
            SNew(SHorizontalBox)

            // 左侧彩色竖条（类型 accent）
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FSlateColor(CardAccent))
                .Padding(3.f, 0.f)
            ]

            // 主体内容
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            .Padding(6.f, 4.f)
            [
                SNew(SVerticalBox)

                // 显示名称行
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)

                    // 警告 / 正常图标
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(0.f, 0.f, 4.f, 0.f)
                    [
                        SNew(STextBlock)
                        .Text(bHasError
                            ? FText::FromString(TEXT("⚠"))
                            : FText::FromString(TEXT("●")))
                        .ColorAndOpacity(FSlateColor(bHasError
                            ? FLinearColor(1.f, 0.7f, 0.f)
                            : CardAccent))
                    ]

                    // 组件名
                    + SHorizontalBox::Slot()
                    .FillWidth(1.f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(Entry->DisplayName)
                        .Font(FAppStyle::GetFontStyle("SmallFont"))
                        .ToolTipText(bHasError ? Entry->ErrorText : FText::GetEmpty())
                    ]
                ]

                // 校验错误文字（可选展开）
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Visibility(bHasError ? EVisibility::Visible : EVisibility::Collapsed)
                    .Text(Entry->ErrorText)
                    .Font(FCoreStyle::GetDefaultFontStyle("Italic", 7))
                    .ColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.7f, 0.f)))
                    .AutoWrapText(true)
                ]
            ]

            // 删除按钮
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0.f, 0.f, 4.f, 0.f)
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), "NoBorder")
                .ToolTipText(LOCTEXT("RemoveCard", "移除此组件"))
                .OnClicked_Lambda([RemoveFn, Index]() -> FReply
                {
                    RemoveFn(Index);
                    return FReply::Handled();
                })
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("✕")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                    .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                ]
            ]
        ];
}

// ── 选中 ─────────────────────────────────────────────────────────────────────

void SBatchPipelineView::SelectEntry(FEntryPtr Entry)
{
    SelectedEntry = Entry;
    if (Entry.IsValid())
    {
        OnSelectionChanged.ExecuteIfBound(Entry->Component);
    }
}

// ── 添加 / 删除 ──────────────────────────────────────────────────────────────

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
    FScopedTransaction Tx(LOCTEXT("RemoveScanner", "删除扫描器"));
    Asset->Modify();
    Asset->RemoveScanner(Index);
    Asset->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnFilterRemove(int32 Index)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;
    FScopedTransaction Tx(LOCTEXT("RemoveFilter", "删除过滤器"));
    Asset->Modify();
    Asset->RemoveFilter(Index);
    Asset->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnProcessorRemove(int32 Index)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;
    FScopedTransaction Tx(LOCTEXT("RemoveProcessor", "删除处理器"));
    Asset->Modify();
    Asset->RemoveProcessor(Index);
    Asset->MarkPackageDirty();
    Refresh();
}

#undef LOCTEXT_NAMESPACE
