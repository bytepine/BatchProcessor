// Fill out your copyright notice in the Description page of Project Settings.


#include "Processor_Iterators.h"

bool UProcessor_Iterators::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Pointer, Struct);
}

void UProcessor_Iterators::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);
	
	SubProcessors.Append(Processors);
}

