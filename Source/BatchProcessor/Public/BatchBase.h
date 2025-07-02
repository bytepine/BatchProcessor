// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchBase.generated.h"

class UCommonProcessorBase;
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
	void Stop();
protected:
	/**
	 * 批处理开始
	 */
	virtual void OnStart();

	/**
	 * 批处理完成
	 */
	virtual void OnFinish();

	/**
	 * 搜索器实例
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="扫描", meta=(DisplayName="搜索器"))
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
	TArray<UCommonProcessorBase*> Processors;
};
