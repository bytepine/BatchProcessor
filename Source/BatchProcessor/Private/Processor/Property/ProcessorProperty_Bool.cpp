// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorProperty_Bool.h"

#include "BatchFunctionLibrary.h"

bool UProcessorProperty_Bool::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);
	
	const EBatchSetPropertyResult SetResult = UBatchFunctionLibrary::SetProperty(PropertyName, Variable, Value);
	
	LogResult(SetResult);
	
	bResult |= SetResult == EBatchSetPropertyResult::Success;
	
	return bResult;
}
