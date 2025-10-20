// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchContext.h"

#include "BatchDefine.h"

#define MAX_LOAD_COUNT 5

UClass* IBatchScratchPadInterface::GetScratchPadClass() const
{
	return UBatchScratchPad::StaticClass();
}

UBatchContext::UBatchContext(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Count(0)
	, Total(0)
{
	AssetSet.Empty();
	LoadArray.Empty();
	ScratchPadMap.Empty();
}

void UBatchContext::Initialized(const TSet<FAssetData>& InAssetSet)
{
	Total = InAssetSet.Num();
	
	AssetSet = InAssetSet;
	LoadArray = InAssetSet.Array();
}

int32 UBatchContext::GetPendingArray(TArray<FSoftObjectPath>& PendingArray)
{
	const int32 LoadCount = LoadArray.Num() > MAX_LOAD_COUNT ? MAX_LOAD_COUNT : LoadArray.Num();
	for (int i = 0; i < LoadCount; ++i)
	{
		PendingArray.Add(LoadArray.Pop().ToSoftObjectPath());
	}
	return LoadCount;
}

int32 UBatchContext::AddCount()
{
	return ++Count;
}

bool UBatchContext::IsLoadFinish() const
{
	return Count >= Total;
}

FString UBatchContext::GetProgress() const
{
	const float Percent = static_cast<float>(Count) / static_cast<float>(Total) * 100.f;
	const FString Progress = FString::Printf(TEXT("%d/%d (%.1f%%)"), Count, Total, Percent);
	return Progress;
}

template <typename BatchScratchPadType>
BatchScratchPadType* UBatchContext::GetScratchPad(const IBatchScratchPadInterface* Owner)
{
	if (!Owner)
	{
		return nullptr;
	}
	
	UBatchScratchPad* ScratchPad = nullptr;
	if (const int64 UID = Owner->GetUID(); !ScratchPadMap.Contains(UID))
	{
		ScratchPad = NewObject<UBatchScratchPad>(this, Owner->GetScratchPadClass());
		ScratchPadMap.Add(UID, ScratchPad);
	}
	else
	{
		ScratchPad = ScratchPadMap[UID];
	}
	return Cast<BatchScratchPadType>(ScratchPad);
}