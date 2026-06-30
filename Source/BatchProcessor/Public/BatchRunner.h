// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "UObject/Object.h"
#include "BatchBase.h"
#include "BatchRunner.generated.h"

class UBatchBase;
class UBatchContext;
class IBatchProgressReporter;
class IBatchAssetSaver;
struct FBatchTarget;

/**
 * 批处理运行实例
 *
 * 承载单次批处理的全部运行态（状态机 / 异步加载器 / 进度通知 / 上下文）。
 * 由 UBatchBase::Start 通过 NewObject 创建，配置数据仍读取自 CDO（UBatchBase）。
 * 这样 CDO 只作为不可变的配置模板，运行态不再污染 CDO，支持安全的重入与回调保活。
 */
UCLASS()
class BATCHPROCESSOR_API UBatchRunner : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 启动一次批处理运行
	 * @param InConfig 配置来源（通常为批处理蓝图的 CDO）
	 */
	void Run(UBatchBase* InConfig);

	/**
	 * 请求停止当前运行（在下一批次之间生效）
	 */
	void RequestStop();

	/**
	 * 是否正在运行中
	 */
	bool IsRunning() const;

	/**
	 * 注入进度反馈实现（未注入时 Run 内默认使用 Slate 实现）
	 */
	void SetProgressReporter(const TSharedRef<IBatchProgressReporter>& InReporter);

	/**
	 * 注入资产保存实现（未注入时 Run 内默认使用落盘实现）
	 */
	void SetAssetSaver(const TSharedRef<IBatchAssetSaver>& InSaver);

private:
	/**
	 * 批处理开始
	 */
	void OnStart();

	/**
	 * 批处理停止
	 */
	void OnStop();

	/**
	 * 批处理执行（请求下一批资产异步加载）
	 */
	void OnProcessing();

	/**
	 * 资产加载完成回调
	 * @param PendingArray 本批资产路径
	 */
	void OnAssetLoaded(TArray<FSoftObjectPath> PendingArray);

	/**
	 * 处理单个资产
	 * @param Target 加载资产封装
	 * @return 是否有修改
	 */
	bool ProcessAssets(const FBatchTarget& Target);

	/**
	 * 批处理完成
	 */
	void OnFinish();

	/**
	 * 配置来源（CDO）
	 */
	UPROPERTY()
	UBatchBase* Config;

	/**
	 * 运行上下文
	 */
	UPROPERTY()
	UBatchContext* Context;

	/**
	 * 批处理状态
	 */
	EBatchStatus Status = EBatchStatus::Idle;

	/**
	 * 资产加载管理器
	 */
	FStreamableManager StreamableManager;

	/**
	 * 进度反馈实现
	 */
	TSharedPtr<IBatchProgressReporter> ProgressReporter;

	/**
	 * 资产保存实现
	 */
	TSharedPtr<IBatchAssetSaver> AssetSaver;
};
