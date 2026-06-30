// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchPreviewList.h"
#include "BatchAsset.h"
#include "BatchRunner.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Modules/ModuleManager.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#define LOCTEXT_NAMESPACE "SBatchPreviewList"

void SBatchPreviewList::Construct(const FArguments& InArgs)
{
    AssetPtr = InArgs._Asset;

    SAssignNew(ListView, SListView<FAssetDataPtr>)
        .ListItemsSource(&PreviewItems)
        .OnGenerateRow(SListView<FAssetDataPtr>::FOnGenerateRow::CreateSP(
            this, &SBatchPreviewList::GenerateRow))
        .OnMouseButtonDoubleClick(SListView<FAssetDataPtr>::FOnMouseButtonDoubleClick::CreateSP(
            this, &SBatchPreviewList::OnRowDoubleClicked));

    ChildSlot
    [
        SNew(SVerticalBox)

        // 顶部：说明 + 刷新按钮
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 6.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(this, &SBatchPreviewList::GetHeaderText)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(LOCTEXT("Scan", "扫描预览"))
                .ToolTipText(LOCTEXT("Scan_Tip", "仅执行 Scanner pre-load，不加载资产内容，零副作用"))
                .OnClicked(this, &SBatchPreviewList::OnPreviewClicked)
            ]
        ]

        // 列表
        + SVerticalBox::Slot()
        .FillHeight(1.f)
        [
            ListView.ToSharedRef()
        ]
    ];
}

void SBatchPreviewList::RefreshPreview()
{
    UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return;

    TArray<FAssetData> Results;
    UBatchRunner::PreviewMatchedAssets(Asset, Results);

    PreviewItems.Reset(Results.Num());
    for (const FAssetData& AD : Results)
    {
        PreviewItems.Add(MakeShared<FAssetData>(AD));
    }
    bScanned = true;
    if (ListView) ListView->RequestListRefresh();
}

FReply SBatchPreviewList::OnPreviewClicked()
{
    RefreshPreview();
    return FReply::Handled();
}

TSharedRef<ITableRow> SBatchPreviewList::GenerateRow(
    FAssetDataPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    const FString ClassName = Item->AssetClassPath.GetAssetName().ToString();
    const FString ObjectPath = Item->GetObjectPathString();

    return SNew(STableRow<FAssetDataPtr>, OwnerTable)
        .Padding(FMargin(4.f, 2.f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(0.35f)
            [
                SNew(STextBlock)
                .Text(FText::FromName(Item->AssetName))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(0.25f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(ClassName))
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
            + SHorizontalBox::Slot()
            .FillWidth(0.40f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(ObjectPath))
                .ToolTipText(FText::FromString(ObjectPath))
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
        ];
}

void SBatchPreviewList::OnRowDoubleClicked(FAssetDataPtr Item)
{
    if (!Item.IsValid()) return;

    // 在 Content Browser 中同步选中该资产
    if (FModuleManager::Get().IsModuleLoaded("ContentBrowser"))
    {
        // Use IContentBrowserSingleton to sync
        FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser")
            .Get().SyncBrowserToAssets(TArray<FAssetData>{ *Item });
    }
}

FText SBatchPreviewList::GetHeaderText() const
{
    if (!bScanned)
    {
        return LOCTEXT("NotScanned", "点击「扫描预览」查看候选资产");
    }
    return FText::Format(
        LOCTEXT("ScannedFmt", "候选资产：{0} 个"),
        PreviewItems.Num());
}

#undef LOCTEXT_NAMESPACE
