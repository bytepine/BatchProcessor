// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchDefine.generated.h"

class UBlueprint;

DECLARE_LOG_CATEGORY_EXTERN(LogBatchProcessor, Log, All);

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

/**
 * 批处理目标
 *
 * 统一抽象「被批处理的对象」：可能是 UBlueprint（变量根=GeneratedClass CDO），
 * 也可能是 DataAsset / Material 等普通资产（变量根=资产自身）。
 * 组件链统一面向 FBatchTarget，不再硬编码 UBlueprint。
 */
USTRUCT(BlueprintType)
struct BATCHPROCESSOR_API FBatchTarget
{
	GENERATED_BODY()

	FBatchTarget() : Asset(nullptr) {}
	explicit FBatchTarget(UObject* InAsset) : Asset(InAsset) {}

	/** 是否有效 */
	bool IsValid() const;

	/** 原始资产对象 */
	UObject* GetAsset() const { return Asset; }

	/** 若为蓝图则返回 UBlueprint，否则返回 nullptr */
	UBlueprint* GetBlueprint() const;

	/**
	 * 获取「生成类」：
	 * 蓝图取 Blueprint->GeneratedClass，其它资产取 Asset->GetClass()。
	 */
	UClass* GetGeneratedClass() const;

	/**
	 * 获取「变量根对象」：
	 * 蓝图取 GeneratedClass 的 CDO，其它资产取 Asset 自身。
	 * 反射读写以此为入口。
	 */
	UObject* GetVariableObject() const;

	/**
	 * 获取「保存对象」：
	 * 落盘/标脏的对象，恒为 Asset 本身。
	 */
	UObject* GetSaveObject() const { return Asset; }

	/** 由变量根构造 FBatchVariable（无效时返回空 Variable） */
	FBatchVariable MakeVariable() const;

	/** 资产名 */
	FString GetName() const;

	/** 资产完整路径名 */
	FString GetPathName() const;

	UPROPERTY(BlueprintReadOnly, Category = "批处理")
	TObjectPtr<UObject> Asset;
};

/**
 * 批处理结果
 *
 * 由 UBatchRunner 在处理过程中累积，供统计器（如 UProcessor_Usage）与
 * 进度反馈在 OnFinish 时读取。统一取代各处理器自行收集的散落状态。
 */
USTRUCT(BlueprintType)
struct BATCHPROCESSOR_API FBatchResult
{
	GENERATED_BODY()

	/** 是否有任何修改 */
	bool bModified = false;

	/** 已处理（通过过滤器并执行了处理器）的资产数 */
	int32 ProcessedCount = 0;

	/** 实际产生修改的资产数 */
	int32 ModifiedCount = 0;

	/** 被过滤器跳过的资产数 */
	int32 SkippedCount = 0;

	/** 处理失败的资产数 */
	int32 FailedCount = 0;

	/** 被处理的资产路径列表 */
	TArray<FString> TouchedAssets;

	/** 记录一个被处理的资产 */
	void AddProcessed(const FString& Path)
	{
		++ProcessedCount;
		TouchedAssets.Add(Path);
	}

	/** 标记有修改 */
	void MarkModified()
	{
		bModified = true;
		++ModifiedCount;
	}

	/** 记录跳过 */
	void AddSkipped() { ++SkippedCount; }

	/** 记录失败 */
	void AddFailed() { ++FailedCount; }

	/** 生成摘要文本；DryRun 模式下"修改"改为"会修改"以区分实际落盘 */
	FString GetSummary(bool bDryRun = false) const;
};