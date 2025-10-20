// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_Material.generated.h"

/**
 * 材质修改器
 */
UCLASS(DisplayName="材质修改器")
class BATCHPROCESSOR_API UProcessorProperty_Material : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const override;
	
	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标值")
	TSoftObjectPtr<UMaterialInterface> Value;
};
