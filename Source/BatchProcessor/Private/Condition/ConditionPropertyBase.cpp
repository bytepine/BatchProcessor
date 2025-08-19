// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionPropertyBase.h"

#include "BatchFunctionLibrary.h"

bool UConditionPropertyBase::FindProperty(void* Pointer, const UStruct* Struct, void*& FindPoint,
                                          FProperty*& FindProperty) const
{
	return UBatchFunctionLibrary::FindProperty(PropertyName, Pointer, Struct, FindPoint, FindProperty);
}
