// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorProperty_Object.h"

#include "BatchFunctionLibrary.h"

bool UProcessorProperty_Object::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);

	const EBatchSetPropertyResult SetResult = UBatchFunctionLibrary::SetProperty(PropertyName, Variable, Value.Get());

	LogResult(SetResult);

	bResult |= SetResult == EBatchSetPropertyResult::Success;

	return bResult;
}
