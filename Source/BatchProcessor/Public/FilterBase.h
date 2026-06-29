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

protected:
	virtual bool OnShouldKeep(const FBatchTarget& Target) const;

	/** 取反：将「匹配则保留」翻转为「匹配则排除」 */
	UPROPERTY(EditDefaultsOnly, Category = "参数", meta=(DisplayName = "取反"))
	uint8 bInvert : 1;
};

