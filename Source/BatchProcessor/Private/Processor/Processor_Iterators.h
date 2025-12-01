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
	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const override;
	
	virtual void GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const override;
	
	bool DoProcessor(const UBlueprint* Assets, UBatchContext* Context, const FBatchProperty& Property) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "属性迭代", meta=(DisplayName = "成员属性"))
	FString PropertyName;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "属性迭代", meta=(DisplayName = "处理器"))
	TArray<UProcessorBase*> Processors;
};
