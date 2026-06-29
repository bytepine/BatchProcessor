// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorBlueprintBase.h"

void UProcessorBlueprintBase::OnStart(UBatchContext* Context) const
{
	Super::OnStart(Context);
	
	OnStartBP(Context);
}

bool UProcessorBlueprintBase::OnProcessing(const FBatchTarget& Target, UBatchContext* Context,
	const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);
	
	bResult |= OnProcessingBP(Target, Context, Variable);
	
	return bResult;
}

void UProcessorBlueprintBase::OnFinish(UBatchContext* Context) const
{
	Super::OnFinish(Context);
	
	OnFinishBP(Context);
}

void UProcessorBlueprintBase::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);
	
	GetSubProcessorsBP(SubProcessors);
}
