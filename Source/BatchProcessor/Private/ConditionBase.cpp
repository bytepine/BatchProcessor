// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionBase.h"

UConditionBase::UConditionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bNegation(false)
{
	
}

bool UConditionBase::CheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = OnCheckCondition(Assets, Context, Variable);

	if (bNegation)
	{
		bResult = !bResult;
	}
	
	return bResult;
}

bool UConditionBase::OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
{
	return false;
}
