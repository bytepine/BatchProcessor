// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchDefine.generated.h"

USTRUCT(BlueprintType)
struct BATCHPROCESSOR_API FBatchVariable
{
	GENERATED_BODY()

	FBatchVariable()
		: Address(nullptr)
		, Struct(nullptr)
	{
	}

	explicit FBatchVariable(UObject* Object)
		: Address(Object)
		, Struct(nullptr)
	{
		if (Object)
		{
			Struct = Object->GetClass();
		}
	}
	
	FBatchVariable(void* Address, const UStruct* Struct)
		: Address(Address)
		, Struct(Struct)
	{
	}

	bool IsValid() const
	{
		return Address != nullptr && Struct != nullptr;
	}
	
	/**
	 * 变量地址
	 */
	void* Address;

	/**
	 * 变量结构
	 */
	const UStruct* Struct;
};

USTRUCT(BlueprintType)
struct BATCHPROCESSOR_API FBatchProperty
{
	GENERATED_BODY()

	FBatchProperty()
		: Address(nullptr)
		, Property(nullptr)
	{
	}
	
	FBatchProperty(void* Address, const FProperty* Property)
		: Address(Address)
		, Property(Property)
	{
	}

	bool IsValid() const
	{
		return Address != nullptr && Property != nullptr;
	}
	
	/**
	 * 变量地址
	 */
	void* Address;

	/**
	 * 变量结构
	 */
	const FProperty* Property;
};

/**
 * 批处理便笺簿
 */
UCLASS(Abstract)
class BATCHPROCESSOR_API UBatchScratchPad : public UObject
{
	GENERATED_BODY()
};

/**
 * 批处理便笺簿接口
 */
UINTERFACE(BlueprintType)
class BATCHPROCESSOR_API UBatchScratchPadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 批处理便笺簿接口
 */
class BATCHPROCESSOR_API IBatchScratchPadInterface
{
	GENERATED_BODY()

public:
	virtual int64 GetUID() const = 0;
	
	virtual UClass* GetScratchPadClass() const;
};