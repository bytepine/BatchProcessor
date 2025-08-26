// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/Property/ConditionPropertyContainerBase.h"
#include "ConditionPropertyContainer_Int.generated.h"

/**
 * 
 */
UCLASS(DisplayName="检查整数容器")
class BATCHPROCESSOR_API UConditionPropertyContainer_Int : public UConditionPropertyContainerBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct) override;

	bool CheckIntArray(const TArray<int64>& IntArray);
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TArray<int64> Values;
};
