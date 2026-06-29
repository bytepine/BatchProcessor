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
	if (Status != EBatchStatus::Processing) return;

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

	if (const int32 Count = Context->GetPendingArray(PendingArray); Count > 0)
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

		if (UObject* LoadedObject = TargetPath.ResolveObject(); IsValid(LoadedObject))
		{
			const FBatchTarget Target(LoadedObject);
			if (ProcessAssets(Target))
			{
				// 通过注入的保存器持久化（默认落盘 / 试运行仅记录）
				AssetSaver->SaveAsset(Target.GetSaveObject());
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

	// 记录已处理资产
	Context->GetResult().AddProcessed(Target.GetPathName());

	const bool bResult = UBatchFunctionLibrary::DoProcessors(Config->Processors, Target, Context, Variable);
	if (bResult)
	{
		Context->GetResult().MarkModified();
	}

	return bResult;
}

void UBatchRunner::OnFinish()
{
	Status = EBatchStatus::Idle;

	const FBatchResult& Result = Context->GetResult();
	ProgressReporter->OnFinished(true, FString::Printf(TEXT("批处理完成: %s"), *Result.GetSummary()));

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
