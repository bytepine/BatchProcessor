﻿// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "ProcessorPropertyBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UProcessorPropertyBase : public UProcessorBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="属性名", meta=(DisplayPriority=0))
	FString PropertyName;
};
