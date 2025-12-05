// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "ProcessorBlueprintBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract, EditInlineNew)
class BATCHPROCESSOR_API UProcessorBlueprintBase : public UProcessorBase
{
	GENERATED_BODY()
	
protected:
	/**
	 * @param Context 上下文
	 * 批处理开始
	 */
	virtual void OnStart(UBatchContext* Context) const override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnStart"))
	void OnStartBP(UBatchContext* Context) const;
	
	/**
	 * 处理逻辑
	 * @param Assets 蓝图资产
	 * @param Context 上下文
	 * @param Variable 变量
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnProcessing"))
	bool OnProcessingBP(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const;
	
	/**
	 * @param Context 上下文
	 * 批处理完成
	 */
	virtual void OnFinish(UBatchContext* Context) const override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnFinish"))
	void OnFinishBP(UBatchContext* Context) const;
	
	/**
	 * 获取子处理器
	 * @param SubProcessors 子处理器
	 */
	virtual void GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "GetSubProcessors"))
	void GetSubProcessorsBP(TArray<UProcessorBase*>& SubProcessors) const;
};
