// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorBase.h"

void UProcessorBase::Start() const
{
	OnStart();

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (UProcessorBase* Processor : SubProcessors)
	{
		Processor->Start();
	}
}

bool UProcessorBase::Processing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const
{
	return OnProcessing(Blueprint, Pointer, Struct);
}

void UProcessorBase::Finish() const
{
	OnFinish();

	TArray<UProcessorBase*> SubProcessors;
	GetSubProcessors(SubProcessors);
	for (UProcessorBase* Processor : SubProcessors)
	{
		Processor->Finish();
	}
}

void UProcessorBase::OnStart() const
{
	
}

bool UProcessorBase::OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const
{
	return false;
}

void UProcessorBase::OnFinish() const
{
	
}

void UProcessorBase::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	
}
