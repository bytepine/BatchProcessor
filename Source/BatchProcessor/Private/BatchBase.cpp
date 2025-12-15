// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

#include "BatchContext.h"
#include "BatchDefine.h"
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

	UBatchContext* Context = NewObject<UBatchContext>();
	Context->AddToRoot();
	
	// 搜索资产
	TSet<FAssetData> Assets;
	for (const UScannerBase* Scanner : Scanners)
	{
		Scanner->ScannerAssets(Assets);
	}

	Context->Initialized(Assets);
	
	// 批处理开始
	for (const UProcessorBase* Processor : Processors)
	{
		Processor->Start(Context);
	}
	
	// 处理资产
	if (Context->GetTotal())
	{
		OnProcessing(Context);
	}
	else
	{
		OnFinish(Context);
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

void UBatchBase::OnProcessing(UBatchContext* Context)
{
	if (Status == EBatchStatus::Stop)
	{
		OnStop();
		return;
	}
	
	TArray<FSoftObjectPath> PendingArray;

	if (const int32 Count = Context->GetPendingArray(PendingArray); Count > 0)
	{
		StreamableManager.RequestAsyncLoad(PendingArray, 
			FStreamableDelegate::CreateUObject(this, &UBatchBase::OnAssetLoaded, Context, PendingArray));
	}
	else
	{
		OnFinish(Context);
	}
}

void UBatchBase::OnAssetLoaded(UBatchContext* Context, TArray<FSoftObjectPath> PendingArray)
{
	for (auto& TargetPath : PendingArray)
	{
		Context->AddCount();

		if (UBlueprint* LoadedObject = Cast<UBlueprint>(TargetPath.ResolveObject()); IsValid(LoadedObject))
		{
			if (ProcessAssets(Context, LoadedObject))
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
	
	if (Context->IsLoadFinish())
	{
		OnFinish(Context);
	}
	else
	{
		OnProcessing(Context);
	}
}

bool UBatchBase::ProcessAssets(UBatchContext* Context, UBlueprint* Assets)
{
	const FString Progress = Context->GetProgress();
	
	if (!IsValid(Assets))
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnProcessing: AssetsObject is InValid! %s"), *Progress);
		return false;
	}

	// 更新进度通知
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(FString::Printf(TEXT("%s %s"), *Assets->GetName(), *Progress)));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
	
	if (UBatchFunctionLibrary::CheckFilters(Filters, Assets))
	{
		return false;
	}
	
	const FBatchVariable Variable(Assets->GeneratedClass->GetDefaultObject());
	
	bool bResult = false;
	
	bResult |= UBatchFunctionLibrary::DoProcessors(Processors, Assets, Context, Variable);
	
	return bResult;
}

void UBatchBase::OnFinish(UBatchContext* Context)
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
		Processor->Finish(Context);
	}

	Context->RemoveFromRoot();
}
