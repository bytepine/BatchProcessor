// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_Object.generated.h"

/**
 * 对象引用修改器
 *
 * 将目标属性（FObjectProperty）设置为指定的 UObject* 引用。
 * 适用于运行时确定对象引用的场景（如修改某个 Actor 组件引用）。
 * 如需修改资产引用，优先使用 ProcessorProperty_SoftObject（软引用更安全）。
 */
UCLASS(DisplayName="对象引用修改器")
class BATCHPROCESSOR_API UProcessorProperty_Object : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标对象")
	TObjectPtr<UObject> Value;
};
