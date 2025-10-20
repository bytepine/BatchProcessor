// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "UProcessor_Usage.generated.h"

/**
 * 蓝图统计器
 */
UCLASS(DisplayName="蓝图统计器")
class BATCHPROCESSOR_API UUProcessor_Usage : public UProcessorBase
{
	GENERATED_BODY()

protected:
	virtual void OnStart(UBatchContext* Context) const override;

	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const override;
	
	virtual void OnFinish(UBatchContext* Context) const override;
};
