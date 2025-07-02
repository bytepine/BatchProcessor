// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

UBatchBase::UBatchBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UBatchBase::Start()
{
	OnStart();
}

void UBatchBase::Stop()
{
	
}

void UBatchBase::OnStart()
{
	
}

void UBatchBase::OnFinish()
{
	
}
