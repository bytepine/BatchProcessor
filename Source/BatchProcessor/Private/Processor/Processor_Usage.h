// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "Processor_Usage.generated.h"

/**
 * 资产统计器
 *
 * 从 UBatchContext 的 FBatchResult 读取被处理的资产列表，
 * 在批处理结束时弹出汇总对话框。不再自行收集（由 UBatchRunner 统一聚合）。
 */
UCLASS(DisplayName="资产统计器")
class BATCHPROCESSOR_API UProcessor_Usage : public UProcessorBase
{
	GENERATED_BODY()

protected:
	virtual void OnFinish(UBatchContext* Context) const override;
};
