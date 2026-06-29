// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Condition.h"

#include "BatchFunctionLibrary.h"

bool UProcessor_Condition::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);

	if (UBatchFunctionLibrary::CheckConditions(Conditions, bMustPassAllCondition, Target, Context, Variable))
	{
		bResult |= UBatchFunctionLibrary::DoProcessors(Processors, Target, Context, Variable);
	}
	
	return bResult;
}

void UProcessor_Condition::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);

	SubProcessors.Append(Processors);
}
