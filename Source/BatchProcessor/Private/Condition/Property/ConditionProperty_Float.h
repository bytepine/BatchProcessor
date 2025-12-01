// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionProperty_Float.generated.h"

UENUM(BlueprintType)
enum class EFloatComparisonOperators : uint8
{
	Equal			UMETA(DisplayName = "等于"),
	NotEqual		UMETA(DisplayName = "不等于"),
	Greater			UMETA(DisplayName = "大于"),
	Less			UMETA(DisplayName = "小于"),
	GreaterOrEqual	UMETA(DisplayName = "大于等于"),
	LessOrEqual		UMETA(DisplayName = "小于等于")
};

/**
 * 检查浮点数
 */
UCLASS(DisplayName="检查浮点数")
class BATCHPROCESSOR_API UConditionProperty_Float : public UConditionPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) override;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EFloatComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	double Value;
};
