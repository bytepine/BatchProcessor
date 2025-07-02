// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorBase.h"

void UProcessorBase::Start() const
{
	OnStart();
}

bool UProcessorBase::Processing(void* Pointer, const UStruct* Struct) const
{
	return OnProcessing(Pointer, Struct);
}

void UProcessorBase::Finish() const
{
	OnFinish();
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
