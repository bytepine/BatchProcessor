// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/Property/ConditionPropertyContainerBase.h"
#include "ConditionPropertyContainer_String.generated.h"

/**
 * 检查字符串容器
 */
UCLASS(DisplayName="检查字符串容器")
class BATCHPROCESSOR_API UConditionPropertyContainer_String : public UConditionPropertyContainerBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct) override;

	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TArray<FString> Values;
};
