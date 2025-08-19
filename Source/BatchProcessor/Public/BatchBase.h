// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "UObject/Object.h"
#include "BatchBase.generated.h"

#define MAX_LOAD_COUNT 5

class UProcessorBase;
class UFilterBase;
class UScannerBase;

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
	 * 请求异步加载
	 */
	void RequestAsyncLoad();
	
	/**
	 * 资产加载完成
	 * @param PendingArray 资产列表
	 */
	virtual void OnAssetLoaded(TArray<FSoftObjectPath> PendingArray);

	/**
	 * 处理资产
	 * @param LoadedObject 加载资产
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(UBlueprint* LoadedObject);
	
	/**
	 * 批处理完成
	 */
	virtual void OnFinish();

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
	 * 处理计数
	 */
	int32 Count;
	
	/**
	 * 最大计数
	 */
	int32 Total;

	/**
	 * 正在处理中
	 */
	uint8 bProcessing : 1;
	
	FStreamableManager StreamableManager;

	TSharedPtr<SNotificationItem> ProgressNotification;

	/**
	 * 待加载列表
	 */
	TArray<FAssetData> AssetArray;
};
