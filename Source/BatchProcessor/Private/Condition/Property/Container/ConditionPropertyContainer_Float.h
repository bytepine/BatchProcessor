// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/Property/ConditionPropertyContainerBase.h"
#include "ConditionPropertyContainer_Float.generated.h"

/**
 * 检查浮点容器
 */
UCLASS(DisplayName="检查浮点容器")
class BATCHPROCESSOR_API UConditionPropertyContainer_Float : public UConditionPropertyContainerBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct) override;

	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TArray<double> Values;
};
