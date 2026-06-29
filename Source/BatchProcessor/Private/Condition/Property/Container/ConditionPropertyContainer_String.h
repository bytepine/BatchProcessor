// Copyright Byteyang Games, Inc. All Rights Reserved.

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
	virtual bool OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) override;

	bool CheckStringArray(const TArray<FString>& StringArray);
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="目标值")
	TArray<FString> Values;
};
