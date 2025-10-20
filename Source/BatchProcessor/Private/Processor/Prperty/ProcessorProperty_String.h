// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_String.generated.h"

/**
 * 字符串修改器
 */
UCLASS(DisplayName="字符串修改器")
class BATCHPROCESSOR_API UProcessorProperty_String : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	FString Value;
};
