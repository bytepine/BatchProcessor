// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionBase.h"

#include "BatchDefine.h"

UConditionBase::UConditionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bNegation(false)
{
	
}

bool UConditionBase::CheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = OnCheckCondition(Target, Context, Variable);

	if (bNegation)
	{
		bResult = !bResult;
	}
	
	return bResult;
}

bool UConditionBase::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	return false;
}
