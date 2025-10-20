// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_Float.generated.h"

/**
 * 浮点修改器
 */
UCLASS(DisplayName="浮点修改器")
class BATCHPROCESSOR_API UProcessorProperty_Float : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	double Value;
};
