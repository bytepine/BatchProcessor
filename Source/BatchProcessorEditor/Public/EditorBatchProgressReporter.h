// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchProgressReporter.h"

/**
 * 编辑器内嵌控制台的进度 Reporter
 *
 * 在游戏线程上被 UBatchRunner 调用（所有 UE Editor 操作均在游戏线程）。
 * SBatchConsole 通过 RegisterActiveTimer 以 ~10Hz 轮询 bIsRunning 状态，
 * 并在 OnStateChanged 触发时立即请求 Slate 重绘。
 */
class BATCHPROCESSOREDITOR_API FEditorBatchProgressReporter : public IBatchProgressReporter
{
public:
    virtual void OnBegin() override;
    virtual void OnProgress(const FString& Message) override;
    virtual void OnFinished(bool bSuccess, const FString& Message) override;

    // ── 状态（游戏线程读写，无需锁）─────────────────────────────────────────

    bool   bIsRunning       = false;
    bool   bLastSucceeded   = false;
    FString CurrentMessage;
    FString FinishedMessage;

    /** Slate 端在此回调中调用 InvalidatePrepass / MarkVolatileDirty */
    FSimpleMulticastDelegate OnStateChanged;
};
