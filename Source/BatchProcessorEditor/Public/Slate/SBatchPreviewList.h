// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"

class UBatchAsset;

/**
 * 匹配资产预览面板（P2）
 *
 * 点击「预览」按钮后，调用 UBatchRunner::PreviewMatchedAssets 执行
 * pre-load 扫描链路，将候选资产以列表展示。
 * 双击可在 Content Browser 中同步选中资产。
 */
class BATCHPROCESSOREDITOR_API SBatchPreviewList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchPreviewList) {}
        SLATE_ARGUMENT(TWeakObjectPtr<UBatchAsset>, Asset)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** 清空并重新预扫描 */
    void RefreshPreview();

private:
    using FAssetDataPtr = TSharedPtr<FAssetData>;

    FReply OnPreviewClicked();
    TSharedRef<ITableRow> GenerateRow(FAssetDataPtr Item,
                                      const TSharedRef<STableViewBase>& OwnerTable);
    void OnRowDoubleClicked(FAssetDataPtr Item);
    FText GetHeaderText() const;

    TArray<FAssetDataPtr>               PreviewItems;
    TSharedPtr<SListView<FAssetDataPtr>> ListView;
    TWeakObjectPtr<UBatchAsset>          AssetPtr;
    bool                                 bScanned = false;
};
