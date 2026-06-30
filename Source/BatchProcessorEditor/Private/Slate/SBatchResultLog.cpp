// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchResultLog.h"
#include "BatchAsset.h"
#include "ScannerBase.h"
#include "FilterBase.h"
#include "ProcessorBase.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchResultLog"

void SBatchResultLog::Construct(const FArguments& InArgs)
{
    AssetPtr = InArgs._Asset;

    SAssignNew(AssetListView, SListView<FStringPtr>)
        .ListItemsSource(&AssetItems)
        .OnGenerateRow(SListView<FStringPtr>::FOnGenerateRow::CreateSP(
            this, &SBatchResultLog::GenerateAssetRow));

    ChildSlot
    [
        SNew(SVerticalBox)

        // ── 校验横幅 ──────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(ValidationBox, SVerticalBox)
        ]

        // ── 分隔线 ────────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 4.f)
        [
            SNew(SSeparator)
        ]

        // ── 结果摘要 ─────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 4.f)
        [
            SAssignNew(SummaryText, STextBlock)
            .Text(LOCTEXT("NoResult", "尚未执行批处理"))
            .AutoWrapText(true)
        ]

        // ── 分隔线 ────────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(SSeparator)
        ]

        // ── TouchedAssets 列表 ───────────────────────────────────────────────
        + SVerticalBox::Slot()
        .FillHeight(1.f)
        [
            AssetListView.ToSharedRef()
        ]
    ];

    RefreshValidation();
}

void SBatchResultLog::SetResult(const FBatchResult& InResult, bool bSuccess)
{
    LastResult   = InResult;
    bLastSuccess = bSuccess;

    // 更新摘要
    if (SummaryText.IsValid())
    {
        const FString Summary = InResult.GetSummary(!bSuccess /* 失败视同 DryRun 摘要 */);
        const FString Prefix  = bSuccess ? TEXT("✔ ") : TEXT("✘ ");
        SummaryText->SetText(FText::FromString(Prefix + Summary));
        SummaryText->SetColorAndOpacity(
            bSuccess ? FSlateColor(FLinearColor(0.4f, 1.f, 0.5f))
                     : FSlateColor(FLinearColor(1.f, 0.5f, 0.4f)));
    }

    // 更新 TouchedAssets 列表
    AssetItems.Reset(InResult.TouchedAssets.Num());
    for (const FString& Path : InResult.TouchedAssets)
    {
        AssetItems.Add(MakeShared<FString>(Path));
    }
    if (AssetListView) AssetListView->RequestListRefresh();
}

void SBatchResultLog::RefreshValidation()
{
    if (!ValidationBox.IsValid()) return;
    ValidationBox->ClearChildren();

    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;

    TArray<FText> AllErrors;
    for (UScannerBase* S   : Asset->GetScanners())   { if (S) S->ValidateConfig(AllErrors); }
    for (UFilterBase*  F   : Asset->GetFilters())    { if (F) F->ValidateConfig(AllErrors); }
    for (UProcessorBase* P : Asset->GetProcessors()) { if (P) P->ValidateConfig(AllErrors); }

    for (const FText& Err : AllErrors)
    {
        ValidationBox->AddSlot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.1f, 0.8f))
            .Padding(4.f)
            [
                SNew(STextBlock)
                .Text(FText::Format(FText::FromString(TEXT("⚠ {0}")), Err))
                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                .AutoWrapText(true)
            ]
        ];
    }
}

TSharedRef<ITableRow> SBatchResultLog::GenerateAssetRow(
    FStringPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<FStringPtr>, OwnerTable)
        .Padding(2.f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(*Item))
            .ToolTipText(FText::FromString(*Item))
        ];
}

#undef LOCTEXT_NAMESPACE
