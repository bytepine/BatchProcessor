// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionBase.h"

UConditionBase::UConditionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bNegation(false)
{
	
}

bool UConditionBase::CheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = OnCheckCondition(Pointer, Struct);

	if (bNegation)
	{
		bResult = !bResult;
	}
	
	return bResult;
}

bool UConditionBase::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	return false;
}
