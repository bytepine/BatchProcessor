// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "Processor_Condition.generated.h"

class UConditionBase;
/**
 * 条件组合器
 */
UCLASS(DisplayName="条件组合器")
class BATCHPROCESSOR_API UProcessor_Condition : public UProcessorBase
{
	GENERATED_BODY()

protected:
	/**
	 * 批处理开始
	 */
	virtual void OnStart() const override;

	/**
	 * 处理逻辑
	 * @param Pointer 处理对象
	 * @param Struct 处理结构
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(void* Pointer, const UStruct* Struct) const override;
	
	/**
	 * 批处理结束
	 */
	virtual void OnFinish() const override;
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "条件组合", meta=(DisplayName = "条件判断"))
	TArray<UConditionBase*> Conditions;

	UPROPERTY(EditDefaultsOnly, Category = "条件组合", meta=(DisplayName = "条件判断必须全部通过"))
	uint8 bMustPassAllCondition : 1;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "条件组合", meta=(DisplayName = "处理器"))
	TArray<UProcessorBase*> Processors;
};
