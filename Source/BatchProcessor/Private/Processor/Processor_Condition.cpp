// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Condition.h"

#include "BatchFunctionLibrary.h"

bool UProcessor_Condition::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Assets, Context, Variable);

	if (UBatchFunctionLibrary::CheckCondition(Conditions, bMustPassAllCondition, Assets, Context, Variable))
	{
		bResult |= UBatchFunctionLibrary::DoProcessors(Processors, Assets, Context, Variable);
	}
	
	return bResult;
}

void UProcessor_Condition::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);

	SubProcessors.Append(Processors);
}
