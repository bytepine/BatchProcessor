// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcessorBlueprintBase.h"

void UProcessorBlueprintBase::OnStart(UBatchContext* Context) const
{
	Super::OnStart(Context);
	
	OnStartBP(Context);
}

bool UProcessorBlueprintBase::OnProcessing(const UBlueprint* Assets, UBatchContext* Context,
	const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Assets, Context, Variable);
	
	bResult |= OnProcessingBP(Assets, Context, Variable);
	
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
