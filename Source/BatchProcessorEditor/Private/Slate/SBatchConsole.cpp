// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchConsole.h"
#include "BatchAsset.h"
#include "BatchRunner.h"
#include "BatchContext.h"
#include "EditorBatchProgressReporter.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchConsole"

void SBatchConsole::Construct(const FArguments& InArgs)
{
    AssetPtr        = InArgs._Asset;
    Reporter        = InArgs._Reporter;
    OnBatchCompleted = InArgs._OnBatchCompleted;

    ChildSlot
    [
        SNew(SVerticalBox)

        // ── 状态标签 ──────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 6.f, 6.f, 2.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("StatusLabel", "状态："))
                .Font(FAppStyle::Get().GetFontStyle("BoldFont"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(this, &SBatchConsole::GetStatusText)
                .ColorAndOpacity(this, &SBatchConsole::GetStatusColor)
                .Font(FAppStyle::Get().GetFontStyle("BoldFont"))
            ]
        ]

        // ── 操作按钮 ─────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.f, 0.f, 4.f, 0.f)
            [
                SNew(SButton)
                .Text(LOCTEXT("Run", "▶ 运行"))
                .ToolTipText(LOCTEXT("Run_Tip", "执行批处理（实际保存）"))
                .IsEnabled(this, &SBatchConsole::CanRun)
                .OnClicked(this, &SBatchConsole::OnRunClicked)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.f, 0.f, 4.f, 0.f)
            [
                SNew(SButton)
                .Text(LOCTEXT("DryRun", "◎ 试运行"))
                .ToolTipText(LOCTEXT("DryRun_Tip", "试运行，不保存资产"))
                .IsEnabled(this, &SBatchConsole::CanRun)
                .OnClicked(this, &SBatchConsole::OnDryRunClicked)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(LOCTEXT("Stop", "■ 停止"))
                .ToolTipText(LOCTEXT("Stop_Tip", "停止批处理（在批次边界生效）"))
                .IsEnabled(this, &SBatchConsole::CanStop)
                .OnClicked(this, &SBatchConsole::OnStopClicked)
            ]
        ]

        // ── 分隔线 ────────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(SSeparator)
        ]

        // ── 进度条 ────────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(SProgressBar)
            .Percent(this, &SBatchConsole::GetProgressFraction)
        ]

        // ── 进度文字 ─────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(STextBlock)
            .Text(this, &SBatchConsole::GetProgressText)
            .AutoWrapText(true)
        ]

        // ── 分隔线 ────────────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 4.f)
        [
            SNew(SSeparator)
        ]

        // ── 统计计数器 ───────────────────────────────────────────────────────
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(6.f, 2.f)
        [
            SNew(STextBlock)
            .Text(this, &SBatchConsole::GetCounterText)
            .AutoWrapText(true)
        ]
    ];

    // 以 ~10 Hz 轮询状态（仅当 widget 可见时激活）
    RegisterActiveTimer(0.1f, FWidgetActiveTimerDelegate::CreateSP(
        this, &SBatchConsole::RefreshTick));
}

// ── 周期刷新 ─────────────────────────────────────────────────────────────────

EActiveTimerReturnType SBatchConsole::RefreshTick(double /*InCurrentTime*/, float /*InDeltaTime*/)
{
    const bool bNowRunning = IsRunning();

    // 检测 running → idle 的完成边界
    if (bWasRunning && !bNowRunning)
    {
        const bool bSuccess = Reporter.IsValid() ? Reporter->bLastSucceeded : false;
        OnBatchCompleted.ExecuteIfBound(bSuccess);
    }
    bWasRunning = bNowRunning;

    return EActiveTimerReturnType::Continue;
}

// ── 按钮事件 ─────────────────────────────────────────────────────────────────

FReply SBatchConsole::OnRunClicked()
{
    if (UBatchAsset* Asset = AssetPtr.Get())
    {
        Asset->StartWithReporter(Reporter);
    }
    return FReply::Handled();
}

FReply SBatchConsole::OnStopClicked()
{
    if (UBatchAsset* Asset = AssetPtr.Get())
    {
        Asset->Stop();
    }
    return FReply::Handled();
}

FReply SBatchConsole::OnDryRunClicked()
{
    if (UBatchAsset* Asset = AssetPtr.Get())
    {
        Asset->StartWithReporter(Reporter, /*bForceDryRun=*/true);
    }
    return FReply::Handled();
}

// ── 状态查询 ─────────────────────────────────────────────────────────────────

bool SBatchConsole::IsRunning() const
{
    const UBatchAsset* Asset = AssetPtr.Get();
    return Asset && Asset->IsRunning();
}

bool SBatchConsole::CanRun() const  { return !IsRunning(); }
bool SBatchConsole::CanStop() const { return  IsRunning(); }

FText SBatchConsole::GetStatusText() const
{
    if (!AssetPtr.IsValid())
    {
        return LOCTEXT("StatusInvalid", "无效资产");
    }
    if (Reporter.IsValid() && Reporter->bIsRunning)
    {
        return LOCTEXT("StatusRunning", "执行中...");
    }
    if (Reporter.IsValid() && !Reporter->FinishedMessage.IsEmpty())
    {
        return Reporter->bLastSucceeded
            ? LOCTEXT("StatusDone",    "完成")
            : LOCTEXT("StatusStopped", "已停止");
    }
    return LOCTEXT("StatusIdle", "待机");
}

FSlateColor SBatchConsole::GetStatusColor() const
{
    if (Reporter.IsValid())
    {
        if (Reporter->bIsRunning)        return FSlateColor(FLinearColor(0.2f, 0.8f, 0.4f));
        if (Reporter->bLastSucceeded)    return FSlateColor(FLinearColor(0.4f, 0.8f, 1.0f));
    }
    return FSlateColor::UseForeground();
}

FText SBatchConsole::GetProgressText() const
{
    if (Reporter.IsValid() && Reporter->bIsRunning && !Reporter->CurrentMessage.IsEmpty())
    {
        return FText::FromString(Reporter->CurrentMessage);
    }
    if (Reporter.IsValid() && !Reporter->FinishedMessage.IsEmpty())
    {
        return FText::FromString(Reporter->FinishedMessage);
    }
    return FText::GetEmpty();
}

TOptional<float> SBatchConsole::GetProgressFraction() const
{
    const UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return 0.f;

    const UBatchRunner* Runner = Asset->GetActiveRunner();
    if (!Runner) return TOptional<float>(); // indeterminate when done

    const UBatchContext* Ctx = Runner->GetContext();
    if (!Ctx || Ctx->GetTotal() <= 0) return TOptional<float>();

    return static_cast<float>(Ctx->GetCount()) / static_cast<float>(Ctx->GetTotal());
}

FText SBatchConsole::GetCounterText() const
{
    const UBatchAsset* Asset = AssetPtr.Get();
    if (!Asset) return FText::GetEmpty();

    // 运行中读取实时结果，运行后读取快照
    const UBatchRunner* Runner = Asset->GetActiveRunner();
    if (Runner)
    {
        if (const UBatchContext* Ctx = Runner->GetContext())
        {
            const FBatchResult& R = Ctx->GetResult();
            return FText::Format(
                LOCTEXT("Counter_Fmt",
                    "处理 {0}  修改 {1}  跳过 {2}  失败 {3}"),
                R.ProcessedCount, R.ModifiedCount, R.SkippedCount, R.FailedCount);
        }
    }
    else
    {
        const FBatchResult& R = Asset->GetLastResult();
        if (R.ProcessedCount > 0 || R.SkippedCount > 0)
        {
            return FText::Format(
                LOCTEXT("Counter_Last_Fmt",
                    "[上次] 处理 {0}  修改 {1}  跳过 {2}  失败 {3}"),
                R.ProcessedCount, R.ModifiedCount, R.SkippedCount, R.FailedCount);
        }
    }
    return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
