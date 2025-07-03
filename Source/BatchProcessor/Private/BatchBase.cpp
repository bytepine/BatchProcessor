// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

#include "BatchProcessor.h"
#include "FilterBase.h"
#include "ScannerBase.h"
#include "Engine/StreamableManager.h"

UBatchBase::UBatchBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Count(0)
	, Total(0)
{
	
}

void UBatchBase::Start()
{
	AddToRoot();
	
	OnStart();
}

void UBatchBase::Stop()
{
	
}

void UBatchBase::OnStart()
{
	// 搜索资产
	TSet<FAssetData> Assets;
	for (const UScannerBase* Scanner : Scanners)
	{
		Scanner->ScannerAssets(Assets);
	}

	// 过滤资产
	for (const UFilterBase* Filter : Filters)
	{
		Filter->Filter(Assets);
	}

	Total = Assets.Num();
	
	// 处理资产
	if (Total > 0)
	{
		FStreamableManager StreamableManager;
		for (const FAssetData& Asset : Assets)
		{
			const FSoftObjectPath& AssetPath = Asset.ToSoftObjectPath();
			StreamableManager.RequestAsyncLoad(AssetPath, 
				FStreamableDelegate::CreateUObject(this, &UBatchBase::OnAssetLoaded, AssetPath));
		}
	}
	else
	{
		OnFinish();
	}
}

void UBatchBase::OnAssetLoaded(FSoftObjectPath AssetPath)
{
	UObject* Asset = AssetPath.ResolveObject();
	UE_LOG(LogBatchProcessor, Log, TEXT("OnAssetLoaded: %s"), *Asset->GetName());
}

void UBatchBase::OnFinish()
{
	RemoveFromRoot();
}
