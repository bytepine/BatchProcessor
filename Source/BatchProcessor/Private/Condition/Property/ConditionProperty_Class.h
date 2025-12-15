// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchDefine.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionProperty_Class.generated.h"



/**
 * 检查类
 */
UCLASS(DisplayName="检查类")
class BATCHPROCESSOR_API UConditionProperty_Class : public UConditionPropertyBase
{
	GENERATED_BODY()

public:
	explicit UConditionProperty_Class(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual bool OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) override;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EClassComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TSoftClassPtr<UObject> Value;
};
