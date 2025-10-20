// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "UProcessor_Usage.h"

void UUProcessor_Usage::OnStart(UBatchContext* Context) const
{
	Super::OnStart(Context);
}

bool UUProcessor_Usage::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	return Super::OnProcessing(Assets, Context, Variable);
}

void UUProcessor_Usage::OnFinish(UBatchContext* Context) const
{
	Super::OnFinish(Context);
}
