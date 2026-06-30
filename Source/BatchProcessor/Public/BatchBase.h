// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchBase.generated.h"

class UBatchContext;
class UProcessorBase;
class UFilterBase;
class UScannerBase;
class UBatchRunner;

UENUM()
enum class EBatchStatus : uint8
{
	Idle,
	Start,
	Processing,
	Stop
};

/**
 * 批处理基类
 *
 * 仅作为配置模板：扫描器 / 过滤器 / 处理器的内联实例配置存放于此（CDO）。
 * 实际运行态与调度逻辑由 UBatchRunner 承载，避免 CDO 被运行态污染。
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class BATCHPROCESSOR_API UBatchBase : public UObject
{
	GENERATED_UCLASS_BODY()

	/**
	 * 开始批处理
	 */
	void Start();

	/**
	 * 停止批处理
	 */
	UFUNCTION(Blueprintable)
	void Stop();

protected:
	/**
	 * 搜索器实例
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="扫描", meta=(DisplayName="扫描器"))
	TArray<UScannerBase*> Scanners;

	/**
	 * 过滤器实例
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="过滤", meta=(DisplayName="过滤器"))
	TArray<UFilterBase*> Filters;

	/**
	 * 处理器实例
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="处理", meta=(DisplayName="处理器"))
	TArray<UProcessorBase*> Processors;

	/**
	 * 每批异步加载的资产数量（较大值加快吞吐，但会占用更多内存）
	 */
	UPROPERTY(EditDefaultsOnly, Category="扫描", meta=(DisplayName="每批加载数量", ClampMin=1, ClampMax=50))
	int32 BatchSize = 5;

	/**
	 * 试运行：仅执行处理流程并记录将要保存的资产，不实际落盘
	 */
	UPROPERTY(EditDefaultsOnly, Category="保存", meta=(DisplayName="试运行(不保存)"))
	bool bDryRun = false;

private:
	/**
	 * 运行实例完成回调，释放对运行实例的引用
	 * @param Runner 已完成的运行实例
	 */
	void OnRunnerFinished(UBatchRunner* Runner);

	/**
	 * 当前活动的运行实例（运行态全部承载于此）
	 */
	UPROPERTY()
	UBatchRunner* ActiveRunner;

	friend class UBatchRunner;
};
