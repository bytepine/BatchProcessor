// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Iterators.h"

bool UProcessor_Iterators::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return Super::OnProcessing(Assets, Context, Variable);
}

void UProcessor_Iterators::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);
	
	SubProcessors.Append(Processors);
}

