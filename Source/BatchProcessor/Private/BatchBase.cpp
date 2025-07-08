// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

#include "BatchFunctionLibrary.h"
#include "BatchProcessor.h"
#include "FilterBase.h"
#include "ProcessorBase.h"
#include "ScannerBase.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UObject/SavePackage.h"
#include "Widgets/Notifications/SNotificationList.h"

UBatchBase::UBatchBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Count(0)
	, Total(0)
	, bProcessing(false)
{
	
}

void UBatchBase::Start()
{
	AddToRoot();

	// 创建进度通知
	FNotificationInfo Info(FText::FromString(TEXT("批处理进行中...")));
	Info.bFireAndForget = false;
	Info.bUseThrobber = true;
	Info.bUseSuccessFailIcons = false;
	Info.bUseLargeFont = false;
	ProgressNotification = FSlateNotificationManager::Get().AddNotification(Info);
	
	OnStart();
}

void UBatchBase::Stop()
{
	// 标记停止
	bProcessing = false;
}

void UBatchBase::OnStart()
{
	// 标记处理状态
	bProcessing = true;
	
	// 批处理开始
	for (const UCommonProcessorBase* Processor : Processors)
	{
		Processor->Start();
	}
	
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
		RequestAsyncLoad();
	}
	else
	{
		OnFinish();
	}
}

void UBatchBase::RequestAsyncLoad()
{
	TArray<FSoftObjectPath> PendingArray;
	const int32 LoadCount = AssetArray.Num() > MAX_LOAD_COUNT ? MAX_LOAD_COUNT : AssetArray.Num();
	for (int i = 0; i < LoadCount; ++i)
	{
		PendingArray.Add(AssetArray.Pop().ToSoftObjectPath());
	}

	StreamableManager.RequestAsyncLoad(PendingArray, 
				FStreamableDelegate::CreateUObject(this, &UBatchBase::OnAssetLoaded, PendingArray));
}

void UBatchBase::OnAssetLoaded(TArray<FSoftObjectPath> PendingArray)
{
	for (auto& TargetPath : PendingArray)
	{
		Count++;

		if (UBlueprint* LoadedObject = Cast<UBlueprint>(TargetPath.ResolveObject()); IsValid(LoadedObject))
		{
			if (OnProcessing(LoadedObject))
			{
				// 标记包为脏
				if (LoadedObject->MarkPackageDirty())
				{
					if (UPackage* Package = LoadedObject->GetOutermost())
					{
						const FString Filename = FPackageName::LongPackageNameToFilename(
							Package->GetName(),
							FPackageName::GetAssetPackageExtension());
						
						if (GEditor && GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
						{
							GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(LoadedObject);
						}
						
						if (!UPackage::SavePackage(Package, nullptr, *Filename, FSavePackageArgs()))
						{
							UE_LOG(LogBatchProcessor, Error, TEXT("OnAssetLoaded: Save File Failed [%s]"), *Filename);
						}
						else
						{
							UE_LOG(LogBatchProcessor, Log, TEXT("OnAssetLoaded: Save File [%s]"), *Filename);
						}
					}
				}
			}
		}

		StreamableManager.Unload(TargetPath);
	}
	
	if (Count >= Total || !bProcessing)
	{
		OnFinish();
	}
	else
	{
		RequestAsyncLoad();
	}
}

bool UBatchBase::OnProcessing(UBlueprint* LoadedObject)
{
	const float Percent = static_cast<float>(Count) / static_cast<float>(Total) * 100.f;
	const FString Progress = FString::Printf(TEXT("%d/%d (%.1f%%)"), Count, Total, Percent);
	
	if (!IsValid(LoadedObject))
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnProcessing: AssetsObject is InValid! %s"), *Progress);
		return false;
	}
	UObject* CDO = LoadedObject->GeneratedClass->GetDefaultObject();

	bool bResult = false;
	
	bResult |= UBatchFunctionLibrary::DoProcessors(Processors, CDO, CDO->GetClass());

	// 更新进度通知
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(FString::Printf(TEXT("%s %s"), *LoadedObject->GetName(), *Progress)));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
	
	return bResult;
}

void UBatchBase::OnFinish()
{
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->Fadeout();
	}

	// 批处理完成
	for (const UCommonProcessorBase* Processor : Processors)
	{
		Processor->Finish();
	}

	// 标记完成
	bProcessing = false;
	
	RemoveFromRoot();
}
