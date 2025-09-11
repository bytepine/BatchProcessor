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
	, Status(EBatchStatus::Idle)
{
	
}

void UBatchBase::Start()
{
	if (Status != EBatchStatus::Idle) return;
	
	OnStart();
}

void UBatchBase::Stop()
{
	if (Status != EBatchStatus::Processing) return;

	Status = EBatchStatus::Stop;
}

void UBatchBase::OnStart()
{
	Status = EBatchStatus::Start;
	
	// 创建进度通知
	FNotificationInfo Info(FText::FromString(TEXT("批处理开始")));
	Info.bFireAndForget = false;
	Info.bUseThrobber = true;
	Info.bUseSuccessFailIcons = false;
	Info.bUseLargeFont = false;
	ProgressNotification = FSlateNotificationManager::Get().AddNotification(Info);
	
	// 批处理开始
	for (const UProcessorBase* Processor : Processors)
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
		OnProcessing();
	}
	else
	{
		OnFinish();
	}
}

void UBatchBase::OnStop()
{
	Status = EBatchStatus::Idle;
	
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(TEXT("批处理停止")));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Fail);
		ProgressNotification->SetFadeOutDuration(3.0f);
		ProgressNotification->Fadeout();
	}
}

void UBatchBase::OnProcessing()
{
	if (Status == EBatchStatus::Stop)
	{
		OnStop();
		return;
	}
	
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
			if (ProcessAssets(LoadedObject))
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
	
	if (Count >= Total)
	{
		OnFinish();
	}
	else
	{
		OnProcessing();
	}
}

bool UBatchBase::ProcessAssets(UBlueprint* Assets)
{
	const float Percent = static_cast<float>(Count) / static_cast<float>(Total) * 100.f;
	const FString Progress = FString::Printf(TEXT("%d/%d (%.1f%%)"), Count, Total, Percent);
	
	if (!IsValid(Assets))
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnProcessing: AssetsObject is InValid! %s"), *Progress);
		return false;
	}
	UObject* CDO = Assets->GeneratedClass->GetDefaultObject();

	bool bResult = false;
	
	bResult |= UBatchFunctionLibrary::DoProcessors(Processors, CDO, CDO->GetClass());

	// 更新进度通知
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(FString::Printf(TEXT("%s %s"), *Assets->GetName(), *Progress)));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
	
	return bResult;
}

void UBatchBase::OnFinish()
{
	Status = EBatchStatus::Idle;
	
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(TEXT("批处理完成")));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Success);
		ProgressNotification->SetFadeOutDuration(3.0f);
		ProgressNotification->Fadeout();
	}

	// 批处理完成
	for (const UProcessorBase* Processor : Processors)
	{
		Processor->Finish();
	}
}
