// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "UObject/Object.h"
#include "BatchBase.generated.h"

class UBatchContext;
class UProcessorBase;
class UFilterBase;
class UScannerBase;

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
	 * 批处理开始
	 */
	virtual void OnStart();
	
	/**
	 * 批处理停止
	 */
	virtual void OnStop();
	
	/**
	 * 批处理执行
	 * @param Context 上下文
	 */
	virtual void OnProcessing(UBatchContext* Context);
	
	/**
	 * 资产加载完成
	 * @param Context 上下文
	 * @param PendingArray 资产列表
	 */
	virtual void OnAssetLoaded(UBatchContext* Context, TArray<FSoftObjectPath> PendingArray);

	/**
	 * 处理资产
	 * @param Context 上下文
	 * @param Assets 加载资产
	 * @return 是否有修改
	 */
	virtual bool ProcessAssets(UBatchContext* Context, UBlueprint* Assets);
	
	/**
	 * 批处理完成
	 * @param Context 上下文
	 */
	virtual void OnFinish(UBatchContext* Context);

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
private:
	/**
	 * 批处理状态
	 */
	EBatchStatus Status;

	/**
	 * 资产加载管理器
	 */
	FStreamableManager StreamableManager;

	/**
	 * 进度通知
	 */
	TSharedPtr<SNotificationItem> ProgressNotification;
};
