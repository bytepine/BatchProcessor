// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Iterators.h"

bool UProcessor_Iterators::OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Blueprint, Pointer, Struct);
}

void UProcessor_Iterators::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);
	
	SubProcessors.Append(Processors);
}

