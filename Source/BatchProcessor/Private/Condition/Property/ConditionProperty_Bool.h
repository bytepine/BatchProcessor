// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionProperty_Bool.generated.h"

UENUM(BlueprintType)
enum class EBoolComparisonOperators : uint8
{
	Equal		UMETA(DisplayName = "等于"),
	NotEqual	UMETA(DisplayName = "不等于")
};

/**
 * 检查布尔
 */
UCLASS(DisplayName="检查布尔")
class BATCHPROCESSOR_API UConditionProperty_Bool : public UConditionPropertyBase
{
	GENERATED_BODY()
	
protected:
	virtual bool OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable) override;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EBoolComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	uint8 bValue : 1;
};
