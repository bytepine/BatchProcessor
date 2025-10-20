// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BatchFunctionLibrary.generated.h"

struct FBatchProperty;
struct FBatchVariable;
class UBatchContext;
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
	static bool DoProcessor(const UProcessorBase* Processor, const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable);
	
	template<typename BatchProcessorType = UProcessorBase>
	static bool DoProcessors(const TArray<BatchProcessorType*>& Processors, const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable);

	static bool FindProperty(const FString& PropertyName, const FBatchVariable& Variable, FBatchProperty& FindProperty);

	static FProperty* FindPropertyByName(const UStruct* Struct, const FString& PropertyName);
};
