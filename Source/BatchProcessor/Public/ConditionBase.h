// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConditionBase.generated.h"

struct FBatchVariable;
class UBatchContext;
/**
 * 条件器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UConditionBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UConditionBase(const FObjectInitializer& ObjectInitializer);

	bool CheckCondition(UBatchContext* Context, const FBatchVariable& Variable);
protected:
	virtual bool OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable);
	
	UPROPERTY(EditDefaultsOnly, Category = "参数", meta=(DisplayName = "取反"))
	uint8 bNegation : 1;
};
