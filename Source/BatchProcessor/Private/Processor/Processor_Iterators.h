// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "Processor_Iterators.generated.h"

/**
 * 属性迭代器
 */
UCLASS(DisplayName="属性迭代器")
class BATCHPROCESSOR_API UProcessor_Iterators : public UProcessorBase
{
	GENERATED_BODY()

protected:
	/**
	 * 处理逻辑
	 * @param Blueprint 蓝图对象
	 * @param Pointer 处理对象
	 * @param Struct 处理结构
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const override;

	/**
	 * 获取子处理器
	 * @param SubProcessors 子处理器
	 */
	virtual void GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const override;
	
	UPROPERTY(EditDefaultsOnly, Category = "属性迭代", meta=(DisplayName = "成员属性"))
	FString PropertyName;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "条件组合", meta=(DisplayName = "处理器"))
	TArray<UProcessorBase*> Processors;
};
