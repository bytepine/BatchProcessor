// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "FilterBase.h"

#include "BatchDefine.h"

UFilterBase::UFilterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bInvert(false)
{
}

bool UFilterBase::ShouldKeep(const FBatchTarget& Target) const
{
	const bool bKeep = OnShouldKeep(Target);
	return bInvert ? !bKeep : bKeep;
}

bool UFilterBase::OnShouldKeep(const FBatchTarget& Target) const
{
	return true;
}

