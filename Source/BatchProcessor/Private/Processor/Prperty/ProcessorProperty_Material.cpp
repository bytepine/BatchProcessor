// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorProperty_Material.h"

#include "BatchFunctionLibrary.h"

bool UProcessorProperty_Material::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Assets, Context, Variable);
	
	const EBatchSetPropertyResult SetResult = UBatchFunctionLibrary::SetProperty(PropertyName, Variable, FSoftObjectPtr(Value.ToSoftObjectPath()));
	
	LogResult(SetResult);
	
	bResult |= SetResult == EBatchSetPropertyResult::Success;
	
	return bResult;
}
