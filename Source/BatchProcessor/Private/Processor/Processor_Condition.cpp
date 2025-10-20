// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Condition.h"

bool UProcessor_Condition::OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Blueprint, Pointer, Struct);
}

void UProcessor_Condition::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);

	SubProcessors.Append(Processors);
}
