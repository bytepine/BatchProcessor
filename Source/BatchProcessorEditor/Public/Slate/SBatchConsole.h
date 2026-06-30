// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "BatchDefine.h"

class UBatchAsset;
class FEditorBatchProgressReporter;

DECLARE_DELEGATE_OneParam(FOnBatchCompleted, bool /*bSuccess*/);

/**
 * 批处理控制台
 *
 * 包含：
 *   - 运行 / 停止 / 试运行 按钮
 *   - 状态标签（Idle / Running / DryRun）
 *   - 当前资产进度文本
 *   - 进度条（Count / Total）
 *   - 统计计数器（Processed / Modified / Skipped / Failed）
 *
 * 通过 RegisterActiveTimer 以 ~10Hz 轮询状态，无需 IBatchProgressReporter 的主动推送。
 * 批处理完成时触发 OnBatchCompleted 委托，供 Toolkit 通知结果面板。
 */
class BATCHPROCESSOREDITOR_API SBatchConsole : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchConsole) {}
        SLATE_ARGUMENT(TWeakObjectPtr<UBatchAsset>, Asset)
        SLATE_ARGUMENT(TSharedPtr<FEditorBatchProgressReporter>, Reporter)
        SLATE_EVENT(FOnBatchCompleted, OnBatchCompleted)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    EActiveTimerReturnType RefreshTick(double InCurrentTime, float InDeltaTime);

    FReply OnRunClicked();
    FReply OnStopClicked();
    FReply OnDryRunClicked();

    bool IsRunning() const;
    bool CanRun() const;
    bool CanStop() const;

    FText GetStatusText() const;
    FSlateColor GetStatusColor() const;
    FText GetProgressText() const;
    TOptional<float> GetProgressFraction() const;
    FText GetCounterText() const;

    TWeakObjectPtr<UBatchAsset>               AssetPtr;
    TSharedPtr<FEditorBatchProgressReporter>  Reporter;
    FOnBatchCompleted                         OnBatchCompleted;

    /** 上一帧是否正在运行（用于检测 running→idle 的边界，触发完成通知） */
    bool bWasRunning = false;
};
