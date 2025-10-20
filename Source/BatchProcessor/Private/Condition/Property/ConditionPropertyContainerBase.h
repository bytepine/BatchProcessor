// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Condition/ConditionPropertyBase.h"
#include "ConditionPropertyContainerBase.generated.h"

UENUM(BlueprintType)
enum class EBoolContainerComparisonOperators : uint8
{
	Include		UMETA(DisplayName = "包含目标"),
	Included	UMETA(DisplayName = "被目标包含"),
	Equal		UMETA(DisplayName = "等于"),
};

/**
 * 
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UConditionPropertyContainerBase : public UConditionPropertyBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较", meta=(DisplayPriority=1))
	EBoolContainerComparisonOperators ComparisonOperator;
};
