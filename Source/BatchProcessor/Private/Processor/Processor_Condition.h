// Copyright Byteyang Games, Inc. All Rights Reserved.

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
	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const override;
	
	virtual void GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const override;
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "条件组合", meta=(DisplayName = "条件判断"))
	TArray<UConditionBase*> Conditions;

	UPROPERTY(EditDefaultsOnly, Category = "条件组合", meta=(DisplayName = "条件判断必须全部通过"))
	uint8 bMustPassAllCondition : 1;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "条件组合", meta=(DisplayName = "处理器"))
	TArray<UProcessorBase*> Processors;
};
