// Fill out your copyright notice in the Description page of Project Settings.

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
	virtual bool OnProcessing(void* Pointer, const UStruct* Struct) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	double Value;
};
