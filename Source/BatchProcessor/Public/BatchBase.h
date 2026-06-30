// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchDefine.h"
#include "BatchBase.generated.h"

class UBatchContext;
class UProcessorBase;
class UFilterBase;
class UScannerBase;
class UBatchRunner;
struct FBatchResult;
class IBatchProgressReporter;

UENUM(BlueprintType)
enum class EBatchStatus : uint8
{
	Idle,
	Start,
	Processing,
	Stop
};

/**
 * 批处理完成委托
 * @param bSuccess true = 正常完成；false = 用户中途停止
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatchFinished, bool, bSuccess);

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
	 * 开始批处理（蓝图与 C++ 均可调用；重复调用时若已在运行则忽略）
	 */
	UFUNCTION(BlueprintCallable, Category="批处理")
	void Start();

	/**
	 * 停止批处理（在下一批次边界生效）
	 */
	UFUNCTION(BlueprintCallable, Category="批处理")
	void Stop();

	/**
	 * 查询当前是否有批处理正在运行
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="批处理")
	bool IsRunning() const;

	/**
	 * 批处理结束委托（正常完成 bSuccess=true，用户停止 bSuccess=false）
	 * 蓝图可在 Class Defaults 或运行时绑定，以便在处理结束后执行后续逻辑。
	 */
	UPROPERTY(BlueprintAssignable, Category="批处理")
	FOnBatchFinished OnBatchFinished;

	// ── 编辑器/C++ 扩展接口 ────────────────────────────────────────────────────

	/**
	 * 用自定义进度 Reporter 启动批处理（C++ 专用，编辑器工具栏调用）
	 * @param InReporter  自定义进度回调；nullptr 时回退到默认 Slate 通知
	 * @param bForceDryRun 覆盖 bDryRun 设置，强制以试运行模式执行
	 */
	void StartWithReporter(TSharedPtr<IBatchProgressReporter> InReporter, bool bForceDryRun = false);

	/** 读取上一次批处理的结果快照（未执行时为默认值） */
	const FBatchResult& GetLastResult() const { return LastResult; }

	/** 当前活动 Runner（未运行时为 nullptr） */
	const UBatchRunner* GetActiveRunner() const { return ActiveRunner; }

	/** 只读访问 Scanners 列表 */
	const TArray<UScannerBase*>& GetScanners() const { return Scanners; }
	/** 只读访问 Filters 列表 */
	const TArray<UFilterBase*>& GetFilters() const { return Filters; }
	/** 只读访问 Processors 列表 */
	const TArray<UProcessorBase*>& GetProcessors() const { return Processors; }
	/** 当前每批加载数量 */
	int32 GetBatchSize() const { return BatchSize; }
	/** 当前是否启用试运行 */
	bool GetDryRun() const { return bDryRun; }

	// ── 编辑器组件管理（由 SBatchPipelineView 调用，修改后须自行 MarkPackageDirty）──

	void AddScanner(UScannerBase* Scanner);
	void RemoveScanner(int32 Index);
	void MoveScanner(int32 FromIndex, int32 ToIndex);

	void AddFilter(UFilterBase* Filter);
	void RemoveFilter(int32 Index);
	void MoveFilter(int32 FromIndex, int32 ToIndex);

	void AddProcessor(UProcessorBase* Processor);
	void RemoveProcessor(int32 Index);
	void MoveProcessor(int32 FromIndex, int32 ToIndex);

protected:
	/**
	 * 搜索器实例
	 * EditAnywhere：允许在资产实例（UBatchAsset）和蓝图 Class Defaults 中均可编辑
	 */
	UPROPERTY(EditAnywhere, Instanced, Category="扫描", meta=(DisplayName="扫描器"))
	TArray<UScannerBase*> Scanners;

	/**
	 * 过滤器实例
	 */
	UPROPERTY(EditAnywhere, Instanced, Category="过滤", meta=(DisplayName="过滤器"))
	TArray<UFilterBase*> Filters;

	/**
	 * 处理器实例
	 */
	UPROPERTY(EditAnywhere, Instanced, Category="处理", meta=(DisplayName="处理器"))
	TArray<UProcessorBase*> Processors;

	/**
	 * 每批异步加载的资产数量（较大值加快吞吐，但会占用更多内存）
	 */
	UPROPERTY(EditAnywhere, Category="扫描", meta=(DisplayName="每批加载数量", ClampMin=1, ClampMax=50))
	int32 BatchSize = 5;

	/**
	 * 试运行：仅执行处理流程并记录将要保存的资产，不实际落盘
	 */
	UPROPERTY(EditAnywhere, Category="保存", meta=(DisplayName="试运行(不保存)"))
	bool bDryRun = false;

private:
	/**
	 * 运行实例完成回调，释放引用并广播 OnBatchFinished 委托
	 * @param Runner    已完成的运行实例
	 * @param bSuccess  true = 正常完成；false = 用户停止
	 */
	void OnRunnerFinished(UBatchRunner* Runner, bool bSuccess);

	/**
	 * 当前活动的运行实例（运行态全部承载于此）
	 */
	UPROPERTY()
	UBatchRunner* ActiveRunner;

	/** 上一次批处理结果快照（完成或停止时存档，供编辑器读取） */
	FBatchResult LastResult;

	friend class UBatchRunner;
};
