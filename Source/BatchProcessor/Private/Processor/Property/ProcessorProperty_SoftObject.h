// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Processor/ProcessorPropertyBase.h"
#include "ProcessorProperty_SoftObject.generated.h"

/**
 * 软对象引用修改器
 *
 * 将目标属性（FSoftObjectProperty）设置为指定的软引用路径。
 * 软引用不强制加载目标资产，适合批量修改蓝图/DataAsset 中的资产引用字段
 * （如替换动画、粒子、音效等引用）。
 * 与 ProcessorProperty_Material 的区别：本修改器适用于任意 UObject 软引用属性，
 * 而非仅限于 UMaterialInterface。
 */
UCLASS(DisplayName="软对象引用修改器")
class BATCHPROCESSOR_API UProcessorProperty_SoftObject : public UProcessorPropertyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const override;

	UPROPERTY(EditDefaultsOnly, Category="属性修改", DisplayName="目标资产（软引用）")
	FSoftObjectPath Value;
};
