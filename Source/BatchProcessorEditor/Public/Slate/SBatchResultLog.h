// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "BatchDefine.h"

class UBatchAsset;

/**
 * 结果 / 日志面板（P2）
 *
 * 顶部：校验横幅（展示所有组件 ValidateConfig 收集到的错误）
 * 中部：批处理结果摘要（Processed / Modified / Skipped / Failed）
 * 下部：TouchedAssets 可滚动列表
 */
class BATCHPROCESSOREDITOR_API SBatchResultLog : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchResultLog) {}
        SLATE_ARGUMENT(TWeakObjectPtr<UBatchAsset>, Asset)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** 批处理结束后由 Toolkit 调用，刷新显示 */
    void SetResult(const FBatchResult& InResult, bool bSuccess);

    /** 重新收集所有组件的 ValidateConfig 错误，刷新横幅 */
    void RefreshValidation();

private:
    using FStringPtr = TSharedPtr<FString>;

    TSharedRef<ITableRow> GenerateAssetRow(FStringPtr Item,
                                           const TSharedRef<STableViewBase>& OwnerTable);

    TWeakObjectPtr<UBatchAsset>    AssetPtr;
    FBatchResult                   LastResult;
    bool                           bLastSuccess = false;

    TArray<FStringPtr>                   AssetItems;
    TSharedPtr<SListView<FStringPtr>>    AssetListView;

    TSharedPtr<SVerticalBox>             ValidationBox;
    TSharedPtr<STextBlock>               SummaryText;
};
