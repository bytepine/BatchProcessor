// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorProperty_Int.h"

#include "BatchFunctionLibrary.h"

bool UProcessorProperty_Int::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);
	
	const EBatchSetPropertyResult SetResult = UBatchFunctionLibrary::SetProperty(PropertyName, Variable, Value);
	
	LogResult(SetResult);
	
	bResult |= SetResult == EBatchSetPropertyResult::Success;
	
	return bResult;
}
