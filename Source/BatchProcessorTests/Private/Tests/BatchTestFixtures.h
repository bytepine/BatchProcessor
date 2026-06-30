// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BatchTestFixtures.generated.h"

/**
 * 测试用嵌套结构体（验证 FindProperty 的路径穿透能力）
 */
USTRUCT()
struct FBatchTestNestedStruct
{
	GENERATED_BODY()

	UPROPERTY()
	bool bNestedFlag = false;
};

/**
 * 测试用资产对象，覆盖 bool/int/FString/嵌套结构体四类属性
 */
UCLASS(Transient)
class UBatchTestObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bFlag = false;

	UPROPERTY()
	int32 Count = 0;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FBatchTestNestedStruct Nested;
};
