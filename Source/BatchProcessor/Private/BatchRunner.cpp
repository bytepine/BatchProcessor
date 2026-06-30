// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchRunner.h"

#include "BatchBase.h"
#include "BatchAssetSaver.h"
#include "BatchContext.h"
#include "BatchDefine.h"
#include "BatchFunctionLibrary.h"
#include "BatchProgressReporter.h"
#include "ProcessorBase.h"
#include "ScannerBase.h"
#include "ScopedTransaction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

void UBatchRunner::Run(UBatchBase* InConfig)
{
	if (!IsValid(InConfig)) return;

	Config = InConfig;

	// 未注入时使用默认实现（编辑器交互场景）
	if (!ProgressReporter.IsValid())
	{
		ProgressReporter = MakeShared<FSlateBatchProgressReporter>();
	}
	if (!AssetSaver.IsValid())
	{
		AssetSaver = MakeShared<FDefaultBatchAssetSaver>();
	}

	OnStart();
}

void UBatchRunner::RequestStop()
{
	if (Status == EBatchStatus::Idle) return;

	Status = EBatchStatus::Stop;
}

bool UBatchRunner::IsRunning() const
{
	return Status != EBatchStatus::Idle;
}

void UBatchRunner::SetProgressReporter(const TSharedRef<IBatchProgressReporter>& InReporter)
{
	ProgressReporter = InReporter;
}

void UBatchRunner::SetAssetSaver(const TSharedRef<IBatchAssetSaver>& InSaver)
{
	AssetSaver = InSaver;
}

void UBatchRunner::OnStart()
{
	Status = EBatchStatus::Start;

	// 通知进度反馈：批处理开始
	ProgressReporter->OnBegin();

	Context = NewObject<UBatchContext>(this);

	// 搜索资产
	TSet<FAssetData> Assets;
	for (const UScannerBase* Scanner : Config->Scanners)
	{
		Scanner->ScannerAssets(Assets);
	}

	Context->Initialized(Assets);

	// 批处理开始
	for (const UProcessorBase* Processor : Config->Processors)
	{
		Processor->Start(Context);
	}

	// 处理资产
	if (Context->GetTotal())
	{
		OnProcessing();
	}
	else
	{
		OnFinish();
	}
}

void UBatchRunner::OnStop()
{
	Status = EBatchStatus::Idle;

	ProgressReporter->OnFinished(false, TEXT("批处理停止"));

	if (Config)
	{
		Config->OnRunnerFinished(this);
	}
}

void UBatchRunner::OnProcessing()
{
	if (Status == EBatchStatus::Stop)
	{
		OnStop();
		return;
	}

	Status = EBatchStatus::Processing;

	TArray<FSoftObjectPath> PendingArray;

	if (const int32 Count = Context->GetPendingArray(PendingArray, Config->BatchSize); Count > 0)
	{
		StreamableManager.RequestAsyncLoad(PendingArray,
			FStreamableDelegate::CreateUObject(this, &UBatchRunner::OnAssetLoaded, PendingArray));
	}
	else
	{
		OnFinish();
	}
}

void UBatchRunner::OnAssetLoaded(TArray<FSoftObjectPath> PendingArray)
{
	for (auto& TargetPath : PendingArray)
	{
		Context->AddCount();

		UObject* LoadedObject = TargetPath.ResolveObject();
		if (!IsValid(LoadedObject))
		{
			// 异步加载完成但 ResolveObject 失败时的兜底（极端边缘情况）
			LoadedObject = LoadObject<UObject>(nullptr, *TargetPath.ToString());
		}
		if (!IsValid(LoadedObject))
		{
			UE_LOG(LogBatchProcessor, Warning,
				TEXT("BatchRunner: Failed to load asset [%s], skipping"), *TargetPath.ToString());
			Context->GetResult().AddFailed();
			StreamableManager.Unload(TargetPath);
			continue;
		}
		{
			const FBatchTarget Target(LoadedObject);

			// 若资产在批处理开始前就有未保存的手动修改，跳过以避免覆盖用户数据
			UPackage* Package = LoadedObject->GetOutermost();
			const bool bWasDirty = Package && Package->IsDirty();

			if (bWasDirty)
			{
				UE_LOG(LogBatchProcessor, Warning,
					TEXT("BatchRunner: Skipping [%s] — asset has unsaved changes before batch processing"),
					*Target.GetName());
				Context->GetResult().AddSkipped();
			}
			else if (ProcessAssets(Target))
			{
				// 蓝图资产：先传播默认值变更再落盘，避免子蓝图 / 已有实例在下次编译时回退
				if (UBlueprint* BP = Target.GetBlueprint())
				{
					FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
					FKismetEditorUtilities::CompileBlueprint(BP);
				}

				const EBatchSaveResult SaveResult = AssetSaver->SaveAsset(Target.GetSaveObject());
				switch (SaveResult)
				{
				case EBatchSaveResult::Success:
				case EBatchSaveResult::Skipped: // DryRun：视为 would-modify
					Context->GetResult().MarkModified();
					break;
				case EBatchSaveResult::Failed:
					Context->GetResult().AddFailed();
					break;
				}
			}
		}

		StreamableManager.Unload(TargetPath);
	}

	if (Context->IsLoadFinish())
	{
		OnFinish();
	}
	else
	{
		OnProcessing();
	}
}

bool UBatchRunner::ProcessAssets(const FBatchTarget& Target)
{
	const FString Progress = Context->GetProgress();

	if (!Target.IsValid())
	{
		Context->GetResult().AddFailed();
		UE_LOG(LogBatchProcessor, Error, TEXT("OnProcessing: Target is InValid! %s"), *Progress);
		return false;
	}

	// 更新进度
	ProgressReporter->OnProgress(FString::Printf(TEXT("%s %s"), *Target.GetName(), *Progress));

	if (!UBatchFunctionLibrary::ShouldKeepAll(Config->Filters, Target))
	{
		Context->GetResult().AddSkipped();
		return false;
	}

	const FBatchVariable Variable = Target.MakeVariable();
	if (!Variable.IsValid())
	{
		Context->GetResult().AddFailed();
		UE_LOG(LogBatchProcessor, Error, TEXT("OnProcessing: Variable is InValid! %s"), *Target.GetName());
		return false;
	}

	// 开启 Undo 事务，记录变更前状态；若无修改则取消，保持撤销栈干净
	FScopedTransaction Transaction(FText::FromString(TEXT("批量处理资产")));
	if (UObject* VarObj = Target.GetVariableObject())
	{
		VarObj->Modify();
	}

	// 记录已处理资产
	Context->GetResult().AddProcessed(Target.GetPathName());

	const bool bResult = UBatchFunctionLibrary::DoProcessors(Config->Processors, Target, Context, Variable);

	if (!bResult)
	{
		// 无实际修改，取消事务避免产生空白撤销记录
		Transaction.Cancel();
	}

	return bResult;
}

void UBatchRunner::OnFinish()
{
	Status = EBatchStatus::Idle;

	const FBatchResult& Result = Context->GetResult();
	ProgressReporter->OnFinished(true, FString::Printf(TEXT("批处理完成: %s"), *Result.GetSummary(Config->bDryRun)));

	// 批处理完成
	for (const UProcessorBase* Processor : Config->Processors)
	{
		Processor->Finish(Context);
	}

	if (Config)
	{
		Config->OnRunnerFinished(this);
	}
}
