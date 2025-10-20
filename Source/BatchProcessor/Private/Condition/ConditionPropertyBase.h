// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConditionBase.h"
#include "ConditionPropertyBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UConditionPropertyBase : public UConditionBase
{
	GENERATED_BODY()

protected:
	bool FindProperty(void* Pointer, const UStruct* Struct, void*& FindPoint, FProperty*& FindProperty) const;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="属性名", meta=(DisplayPriority=0))
	FString PropertyName;
};
