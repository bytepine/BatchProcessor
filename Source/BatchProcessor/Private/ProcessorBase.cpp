// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorBase.h"

void UProcessorBase::Start() const
{
	OnStart();

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (const UProcessorBase* Processor : SubProcessors)
	{
		Processor->Start();
	}
}

bool UProcessorBase::Processing(void* Pointer, const UStruct* Struct) const
{
	return OnProcessing(Pointer, Struct);
}

void UProcessorBase::Finish() const
{
	OnFinish();

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (const UProcessorBase* Processor : SubProcessors)
	{
		Processor->Finish();
	}
}

void UProcessorBase::OnStart() const
{
	
}

bool UProcessorBase::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return false;
}

void UProcessorBase::OnFinish() const
{
	
}

void UProcessorBase::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	
}
