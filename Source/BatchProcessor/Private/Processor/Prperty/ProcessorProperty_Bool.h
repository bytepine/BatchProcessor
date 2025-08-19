// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_Bool.generated.h"

/**
 * 布尔修改器
 */
UCLASS(DisplayName="布尔修改器")
class BATCHPROCESSOR_API UProcessorProperty_Bool : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(void* Pointer, const UStruct* Struct) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	bool Value;
};
