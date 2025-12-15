// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BatchFunctionLibrary.generated.h"

class UFilterBase;
class UConditionBase;
struct FBatchProperty;
struct FBatchVariable;
class UBatchContext;
class UBatchBase;
class UProcessorBase;

UENUM(BlueprintType)
enum class EBatchSetPropertyResult : uint8
{
	Success,    // 操作成功
	Failed,     // 操作失败
	Same,       // 属性值相同无需修改
	NotFound,	// 没找到属性
};

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
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const int64 Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const bool Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const float Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const double Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const FString& Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, UObject* Value);
	
	static EBatchSetPropertyResult SetProperty(const FString& PropertyName, const FBatchVariable& Variable, const FSoftObjectPtr& Value);
	
	static bool CheckConditions(const TArray<UConditionBase*>& Conditions, const bool bMustPassAllCondition, const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable);
	
	static bool CheckFilters(const TArray<UFilterBase*>& Filters, const UBlueprint* Blueprint);
};
