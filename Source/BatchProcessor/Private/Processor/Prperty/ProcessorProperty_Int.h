// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_Int.generated.h"

/**
 * 整数修改器
 */
UCLASS(DisplayName="整数修改器")
class BATCHPROCESSOR_API UProcessorProperty_Int : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	int64 Value;
};
