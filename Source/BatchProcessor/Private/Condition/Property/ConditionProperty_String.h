// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionProperty_String.generated.h"

UENUM(BlueprintType)
enum class EStringComparisonOperators : uint8
{
	Equal		UMETA(DisplayName = "等于"),
	NotEqual	UMETA(DisplayName = "不等于"),
	Contains	UMETA(DisplayName = "包含"),
	NotContains	UMETA(DisplayName = "不包含"),
	StartsWith	UMETA(DisplayName = "开头是"),
	EndsWith	UMETA(DisplayName = "结尾是")
};

/**
 * 检查字符串
 */
UCLASS(DisplayName="检查字符串")
class BATCHPROCESSOR_API UConditionProperty_String : public UConditionPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct) override;

	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EStringComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	FString Value;
};
