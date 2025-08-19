// Fill out your copyright notice in the Description page of Project Settings.

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
