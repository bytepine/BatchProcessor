// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConditionBase.generated.h"

/**
 * 条件器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UConditionBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UConditionBase(const FObjectInitializer& ObjectInitializer);

	bool CheckCondition(void* Pointer, const UStruct* Struct);
protected:
	virtual bool OnCheckCondition(void* Pointer, const UStruct* Struct);
	
	UPROPERTY(EditDefaultsOnly, Category = "参数", meta=(DisplayName = "取反"))
	uint8 bNegation : 1;
};
