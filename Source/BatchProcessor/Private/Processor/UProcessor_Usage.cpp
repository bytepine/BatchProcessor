// Fill out your copyright notice in the Description page of Project Settings.


#include "UProcessor_Usage.h"

void UUProcessor_Usage::OnStart() const
{
	Super::OnStart();
}

bool UUProcessor_Usage::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Pointer, Struct);
}

void UUProcessor_Usage::OnFinish() const
{
	Super::OnFinish();
}
