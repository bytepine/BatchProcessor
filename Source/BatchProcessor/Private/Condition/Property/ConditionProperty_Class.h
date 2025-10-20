// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionProperty_Class.generated.h"

UENUM(BlueprintType)
enum class EClassComparisonOperators : uint8
{
	Equal		UMETA(DisplayName = "等于"),
	NotEqual	UMETA(DisplayName = "不等于"),
	Super		UMETA(DisplayName = "父类"),
	Child		UMETA(DisplayName = "子类")
};

/**
 * 检查类
 */
UCLASS(DisplayName="检查类")
class BATCHPROCESSOR_API UConditionProperty_Class : public UConditionPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct) override;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EClassComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TSoftClassPtr<UObject> Value;
};
