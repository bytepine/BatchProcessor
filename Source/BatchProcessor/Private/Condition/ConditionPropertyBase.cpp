// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionPropertyBase.h"
#include "BatchFunctionLibrary.h"

bool UConditionPropertyBase::FindProperty(const FBatchVariable& Variable,
	FBatchProperty& FindProperty) const
{
	return UBatchFunctionLibrary::FindProperty(PropertyName, Variable, FindProperty);
}
