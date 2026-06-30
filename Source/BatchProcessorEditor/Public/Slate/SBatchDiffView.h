// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Dry-run 差异视图（P3 占位）
 *
 * 未来规划：在 Dry-run 运行前后对各资产的属性做快照，
 * 展示"改变前 → 改变后"的属性差异列表。
 * 当前版本为占位 UI，显示说明文字。
 */
class BATCHPROCESSOREDITOR_API SBatchDiffView : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchDiffView) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};
