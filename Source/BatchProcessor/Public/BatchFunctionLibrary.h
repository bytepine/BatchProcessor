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
	static bool DoProcessor(const UProcessorBase* Processor, void* Pointer, const UStruct* Struct);
	
	template<typename SPBatchProcessorType = UProcessorBase>
	static bool DoProcessors(const TArray<SPBatchProcessorType*>& Processors, void* Pointer, const UStruct* Struct);

	static bool FindProperty(const FString& PropertyName, void* Pointer, const UStruct* Struct, void*& TargetPointer, FProperty*& TargetProperty);

	static FProperty* FindPropertyByName(const UStruct* Struct, const FString& PropertyName);
};
