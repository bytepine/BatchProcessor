// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BatchFunctionLibrary.generated.h"

class UBatchBase;
class UProcessorBase;
/**
 * 批处理函数库
 */
UCLASS()
class BATCHPROCESSOR_API UBatchFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool DoProcessor(const UProcessorBase* Processor, void* Point, const UStruct* Struct);
	
	template<typename SPBatchProcessorType = UProcessorBase>
	static bool DoProcessors(const TArray<SPBatchProcessorType*>& Processors, void* Point, const UStruct* Struct);
};
