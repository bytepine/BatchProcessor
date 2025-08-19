// Fill out your copyright notice in the Description page of Project Settings.


#include "Processor_Condition.h"

void UProcessor_Condition::OnStart() const
{
	Super::OnStart();

	for (const auto Processor : Processors)
	{
		Processor->Start();
	}
}

bool UProcessor_Condition::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Pointer, Struct);
}

void UProcessor_Condition::OnFinish() const
{
	Super::OnFinish();

	for (const auto Processor : Processors)
	{
		Processor->Finish();
	}
}
