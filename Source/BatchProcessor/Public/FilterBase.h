// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FilterBase.generated.h"

struct FBatchTarget;

/**
 * 筛选器基类
 *
 * 语义：ShouldKeep 返回 true 表示保留该资产，false 表示排除。
 * 子类实现 OnShouldKeep 返回「是否匹配保留条件」，基类用 bInvert 翻转结果。
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UFilterBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UFilterBase(const FObjectInitializer& ObjectInitializer);

	/**
	 * 是否保留该资产
	 * @return true=保留，false=排除
	 */
	bool ShouldKeep(const FBatchTarget& Target) const;

	/**
	 * 编辑期配置校验（可选实现）
	 * 将发现的问题追加到 OutErrors，无问题则不写入。
	 */
	virtual void ValidateConfig(TArray<FText>& OutErrors) const {}

	/** 备注：仅用于在流水线视图中标注说明，不影响运行逻辑 */
	UPROPERTY(EditAnywhere, Category="通用", meta=(DisplayName="备注", MultiLine=true))
	FString Remark;

protected:
	virtual bool OnShouldKeep(const FBatchTarget& Target) const;

	/** 取反：将「匹配则保留」翻转为「匹配则排除」 */
	UPROPERTY(EditDefaultsOnly, Category = "参数", meta=(DisplayName = "取反"))
	uint8 bInvert : 1;
};

