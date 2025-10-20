// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorBase.h"

void UProcessorBase::Start(UBatchContext* Context) const
{
	OnStart(Context);

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (const UProcessorBase* Processor : SubProcessors)
	{
		Processor->Start(Context);
	}
}

bool UProcessorBase::Processing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return OnProcessing(Assets, Context, Variable);
}

void UProcessorBase::Finish(UBatchContext* Context) const
{
	OnFinish(Context);

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (const UProcessorBase* Processor : SubProcessors)
	{
		Processor->Finish(Context);
	}
}

void UProcessorBase::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	
}

void UProcessorBase::OnStart(UBatchContext* Context) const
{
	
}

bool UProcessorBase::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return false;
}

void UProcessorBase::OnFinish(UBatchContext* Context) const
{
	
}