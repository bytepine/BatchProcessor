// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchDefine.generated.h"

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

	explicit FBatchVariable(const FBatchProperty& Property)
		: Address(Property.Address)
		, Struct(Property.Property->GetOwnerStruct())
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
	UPROPERTY(Transient)
	const UStruct* Struct;
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

UENUM(BlueprintType)
enum class EClassComparisonOperators : uint8
{
	Equal		UMETA(DisplayName = "等于"),
	NotEqual	UMETA(DisplayName = "不等于"),
	Super		UMETA(DisplayName = "父类"),
	Child		UMETA(DisplayName = "子类")
};