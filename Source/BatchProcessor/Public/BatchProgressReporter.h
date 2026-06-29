// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SNotificationItem;

/**
 * 批处理进度反馈接口
 *
 * 把"进度如何呈现"从调度核心中解耦：编辑器下用 Slate 通知，
 * CI / 命令行 / 自动化测试下可注入空实现或自定义日志实现。
 */
class BATCHPROCESSOR_API IBatchProgressReporter
{
public:
	virtual ~IBatchProgressReporter() = default;

	/** 批处理开始 */
	virtual void OnBegin() {}

	/** 进度更新 */
	virtual void OnProgress(const FString& Message) {}

	/** 批处理结束 */
	virtual void OnFinished(bool bSuccess, const FString& Message) {}
};

/**
 * Slate 通知实现（编辑器交互场景）
 */
class BATCHPROCESSOR_API FSlateBatchProgressReporter : public IBatchProgressReporter
{
public:
	virtual void OnBegin() override;
	virtual void OnProgress(const FString& Message) override;
	virtual void OnFinished(bool bSuccess, const FString& Message) override;

private:
	TSharedPtr<SNotificationItem> ProgressNotification;
};

/**
 * 空实现（CI / 命令行 / 测试场景，无 UI）
 */
class BATCHPROCESSOR_API FNullBatchProgressReporter : public IBatchProgressReporter
{
};
