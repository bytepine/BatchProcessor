// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchFunctionLibrary.h"

#include "ProcessorBase.h"

bool UBatchFunctionLibrary::DoProcessor(const UProcessorBase* Processor, void* Point, const UStruct* Struct)
{
	return Processor->Processing(Point, Struct);
}

template <typename SPBatchProcessorType>
bool UBatchFunctionLibrary::DoProcessors(const TArray<SPBatchProcessorType*>& Processors, void* Point,
	const UStruct* Struct)
{
	bool bResult = false;
	
	for (const UProcessorBase* Processor : Processors)
	{
		bResult |= Processor->Processing(Point, Struct);
	}
	
	return bResult;
}