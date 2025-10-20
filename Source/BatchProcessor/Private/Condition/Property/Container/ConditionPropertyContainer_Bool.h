// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/Property/ConditionPropertyContainerBase.h"
#include "ConditionPropertyContainer_Bool.generated.h"

/**
 * 检查布尔容器
 */
UCLASS(DisplayName="检查布尔容器")
class BATCHPROCESSOR_API UConditionPropertyContainer_Bool : public UConditionPropertyContainerBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable) override;

	bool CheckBoolArray(const TArray<bool>& BoolArray) const;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TArray<bool> Values;
};
