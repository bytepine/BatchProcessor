// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConditionBase.generated.h"

struct FBatchVariable;
struct FBatchTarget;
class UBatchContext;
/**
 * 条件器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UConditionBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UConditionBase(const FObjectInitializer& ObjectInitializer);

	bool CheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable);

	/** 编辑期配置校验（可选实现） */
	virtual void ValidateConfig(TArray<FText>& OutErrors) const {}

	/** 备注：仅用于在流水线视图中标注说明，不影响运行逻辑 */
	UPROPERTY(EditAnywhere, Category="通用", meta=(DisplayName="备注", MultiLine=true))
	FString Remark;

protected:
	virtual bool OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable);
	
	UPROPERTY(EditDefaultsOnly, Category = "参数", meta=(DisplayName = "取反"))
	uint8 bNegation : 1;
};
