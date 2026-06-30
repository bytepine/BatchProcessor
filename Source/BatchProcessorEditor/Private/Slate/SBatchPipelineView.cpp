// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchPipelineView.h"

#include "BatchAsset.h"
#include "ScannerBase.h"
#include "FilterBase.h"
#include "ProcessorBase.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchPipelineView"

// ── Construct ────────────────────────────────────────────────────────────────

void SBatchPipelineView::Construct(const FArguments& InArgs)
{
    AssetPtr          = InArgs._Asset;
    OnSelectionChanged = InArgs._OnSelectionChanged;

    ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                BuildSection(LOCTEXT("Scanners", "扫描器"), ScannerEntries, ScannerList,
                    UScannerBase::StaticClass(),
                    [this](int32 I){ OnScannerRemove(I); },
                    [this](UClass* C){ OnScannerAdd(C); })
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 4.f)
            [
                SNew(SSeparator)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                BuildSection(LOCTEXT("Filters", "过滤器"), FilterEntries, FilterList,
                    UFilterBase::StaticClass(),
                    [this](int32 I){ OnFilterRemove(I); },
                    [this](UClass* C){ OnFilterAdd(C); })
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 4.f)
            [
                SNew(SSeparator)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                BuildSection(LOCTEXT("Processors", "处理器"), ProcessorEntries, ProcessorList,
                    UProcessorBase::StaticClass(),
                    [this](int32 I){ OnProcessorRemove(I); },
                    [this](UClass* C){ OnProcessorAdd(C); })
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
        for (auto* Item : Src)
        {
            Dst.Add(Item);
        }
    };

    TArray<UObject*> ScanObjs, FilterObjs, ProcObjs;
    ToObjArray(Asset->GetScanners(),   ScanObjs);
    ToObjArray(Asset->GetFilters(),    FilterObjs);
    ToObjArray(Asset->GetProcessors(), ProcObjs);

    PopulateEntries(ScannerEntries,   ScanObjs);
    PopulateEntries(FilterEntries,    FilterObjs);
    PopulateEntries(ProcessorEntries, ProcObjs);

    if (ScannerList)   ScannerList->RequestListRefresh();
    if (FilterList)    FilterList->RequestListRefresh();
    if (ProcessorList) ProcessorList->RequestListRefresh();
}

void SBatchPipelineView::PopulateEntries(TArray<FEntryPtr>& Out, const TArray<UObject*>& Components)
{
    Out.Reset(Components.Num());
    for (UObject* Comp : Components)
    {
        if (!Comp) continue;

        auto Entry = MakeShared<FPipelineEntry>();
        Entry->Component = Comp;

        // 优先用 DisplayName meta
        const FString DN = Comp->GetClass()->GetMetaData(TEXT("DisplayName"));
        Entry->DisplayName = DN.IsEmpty()
            ? FText::FromString(Comp->GetClass()->GetName())
            : FText::FromString(DN);

        // 收集 ValidateConfig 错误
        TArray<FText> Errors;
        if (UScannerBase* S = Cast<UScannerBase>(Comp))   S->ValidateConfig(Errors);
        if (UFilterBase*  F = Cast<UFilterBase>(Comp))    F->ValidateConfig(Errors);
        if (UProcessorBase* P = Cast<UProcessorBase>(Comp)) P->ValidateConfig(Errors);

        Entry->bHasError = !Errors.IsEmpty();
        if (Entry->bHasError)
        {
            Entry->ErrorText = Errors[0];
        }

        Out.Add(Entry);
    }
}

// ── BuildSection ─────────────────────────────────────────────────────────────

TSharedRef<SWidget> SBatchPipelineView::BuildSection(
    const FText& Title,
    TArray<FEntryPtr>& Entries,
    TSharedPtr<SListView<FEntryPtr>>& OutList,
    UClass* ComponentBaseClass,
    TFunction<void(int32)> RemoveFn,
    TFunction<void(UClass*)> AddFn)
{
    TSharedRef<SListView<FEntryPtr>> ListView =
        SNew(SListView<FEntryPtr>)
        .ListItemsSource(&Entries)
        .SelectionMode(ESelectionMode::Single)
        .OnGenerateRow(SListView<FEntryPtr>::FOnGenerateRow::CreateSP(
            this, &SBatchPipelineView::GenerateRow, RemoveFn))
        .OnSelectionChanged(SListView<FEntryPtr>::FOnSelectionChanged::CreateLambda(
            [this](FEntryPtr Item, ESelectInfo::Type Type)
            {
                OnRowSelected(Item, Type);
            }));

    OutList = ListView;

    return SNew(SVerticalBox)

        // 标题行 + "+" 按钮
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 4.f, 6.f, 2.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(Title)
                .Font(FAppStyle::GetFontStyle("BoldFont"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                BuildAddMenu(ComponentBaseClass, AddFn)
            ]
        ]

        // 列表
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            ListView
        ];
}

// ── GenerateRow ──────────────────────────────────────────────────────────────

TSharedRef<ITableRow> SBatchPipelineView::GenerateRow(
    FEntryPtr Item,
    const TSharedRef<STableViewBase>& OwnerTable,
    TFunction<void(int32)> RemoveFn)
{
    // 找到当前项在哪个 Entries 数组中的 index
    // We capture RemoveFn and Item to find index dynamically
    auto FindAndRemove = [this, Item, RemoveFn]()
    {
        for (int32 i = 0; i < ScannerEntries.Num(); ++i)
        {
            if (ScannerEntries[i] == Item) { RemoveFn(i); return; }
        }
        for (int32 i = 0; i < FilterEntries.Num(); ++i)
        {
            if (FilterEntries[i] == Item) { RemoveFn(i); return; }
        }
        for (int32 i = 0; i < ProcessorEntries.Num(); ++i)
        {
            if (ProcessorEntries[i] == Item) { RemoveFn(i); return; }
        }
    };

    return SNew(STableRow<FEntryPtr>, OwnerTable)
        .Padding(FMargin(2.f, 1.f))
        [
            SNew(SHorizontalBox)

            // 错误图标
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(2.f, 0.f)
            [
                SNew(STextBlock)
                .Text(Item->bHasError ? FText::FromString(TEXT("⚠")) : FText::GetEmpty())
                .ColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.7f, 0.f)))
                .ToolTipText(Item->bHasError ? Item->ErrorText : FText::GetEmpty())
            ]

            // 组件名
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(Item->DisplayName)
            ]

            // 删除按钮
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("✕")))
                .ToolTipText(LOCTEXT("Remove", "删除此组件"))
                .ButtonStyle(FAppStyle::Get(), "NoBorder")
                .OnClicked(FOnClicked::CreateLambda([FindAndRemove]()
                {
                    FindAndRemove();
                    return FReply::Handled();
                }))
            ]
        ];
}

// ── AddMenu ──────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SBatchPipelineView::BuildAddMenu(UClass* BaseClass, TFunction<void(UClass*)> AddFn)
{
    // 收集所有非 Abstract 子类（在 UObject 系统注册的）
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
    SubClasses.Sort([](const UClass& A, const UClass& B)
    {
        return A.GetName() < B.GetName();
    });

    FMenuBuilder MenuBuilder(true, nullptr);
    for (UClass* C : SubClasses)
    {
        const FString DN = C->GetMetaData(TEXT("DisplayName"));
        const FText Label = DN.IsEmpty() ? FText::FromString(C->GetName()) : FText::FromString(DN);

        MenuBuilder.AddMenuEntry(
            Label,
            FText::FromString(C->GetName()),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateLambda([AddFn, C]() { AddFn(C); }))
        );
    }

    return SNew(SComboButton)
        .ButtonContent()
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("＋")))
        ]
        .MenuContent()
        [
            MenuBuilder.MakeWidget()
        ]
        .ContentPadding(FMargin(4.f, 2.f));
}

// ── Add / Remove ─────────────────────────────────────────────────────────────

void SBatchPipelineView::OnScannerAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;

    FScopedTransaction Tx(LOCTEXT("AddScanner", "添加扫描器"));
    Asset->Modify();
    UScannerBase* NewComp = NewObject<UScannerBase>(Asset, Class, NAME_None, RF_Transactional);
    Asset->AddScanner(NewComp);
    Asset->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnFilterAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;

    FScopedTransaction Tx(LOCTEXT("AddFilter", "添加过滤器"));
    Asset->Modify();
    UFilterBase* NewComp = NewObject<UFilterBase>(Asset, Class, NAME_None, RF_Transactional);
    Asset->AddFilter(NewComp);
    Asset->MarkPackageDirty();
    Refresh();
}

void SBatchPipelineView::OnProcessorAdd(UClass* Class)
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset || !Class) return;

    FScopedTransaction Tx(LOCTEXT("AddProcessor", "添加处理器"));
    Asset->Modify();
    UProcessorBase* NewComp = NewObject<UProcessorBase>(Asset, Class, NAME_None, RF_Transactional);
    Asset->AddProcessor(NewComp);
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

void SBatchPipelineView::OnRowSelected(FEntryPtr Item, ESelectInfo::Type /*SelectType*/)
{
    if (Item.IsValid())
    {
        OnSelectionChanged.ExecuteIfBound(Item->Component);
    }
}

#undef LOCTEXT_NAMESPACE
