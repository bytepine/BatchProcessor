// Fill out your copyright notice in the Description page of Project Settings.


#include "Processor_Condition.h"

bool UProcessor_Condition::OnProcessing(void* Pointer, const UStruct* Struct) const
{
	return Super::OnProcessing(Pointer, Struct);
}

void UProcessor_Condition::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);

	SubProcessors.Append(Processors);
}
