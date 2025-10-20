// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Condition.h"

bool UProcessor_Condition::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return Super::OnProcessing(Assets, Context, Variable);
}

void UProcessor_Condition::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);

	SubProcessors.Append(Processors);
}
