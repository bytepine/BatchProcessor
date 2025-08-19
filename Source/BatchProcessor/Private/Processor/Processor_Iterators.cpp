// Fill out your copyright notice in the Description page of Project Settings.


#include "Processor_Iterators.h"

void UProcessor_Iterators::OnStart() const
{
	Super::OnStart();

	for (const auto Processor : Processors)
	{
		Processor->Start();
	}
}

bool UProcessor_Iterators::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Pointer, Struct);
}

void UProcessor_Iterators::OnFinish() const
{
	Super::OnFinish();

	for (const auto Processor : Processors)
	{
		Processor->Finish();
	}
}
