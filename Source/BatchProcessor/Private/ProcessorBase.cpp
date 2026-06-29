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

bool UProcessorBase::Processing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return OnProcessing(Target, Context, Variable);
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

int64 UProcessorBase::GetUID() const
{
	return GetTypeHash(GetName());
}

void UProcessorBase::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	
}

void UProcessorBase::OnStart(UBatchContext* Context) const
{
	
}

bool UProcessorBase::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return false;
}

void UProcessorBase::OnFinish(UBatchContext* Context) const
{
	
}