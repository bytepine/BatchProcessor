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
		// Instanced 数组可能含 null（编辑器删除内联实例 / 类丢失），跳过以防崩溃
		if (!IsValid(Scanner)) continue;
		Scanner->ScannerAssets(Assets);
	}

	Context->Initialized(Assets);

	// 批处理开始
	for (const UProcessorBase* Processor : Config->Processors)
	{
		if (!IsValid(Processor)) continue;
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

	// 中途停止同样要给处理器收尾，避免在 Start 中申请的资源 / ScratchPad 泄漏
	FinalizeProcessors();

	ProgressReporter->OnFinished(false, TEXT("批处理停止"));

	if (Config)
	{
		Config->OnRunnerFinished(this, false);
	}
}

void UBatchRunner::OnProcessing()
{
	// 蹦床循环：正常路径下 RequestAsyncLoad 回调来自后续 tick（自然展栈，循环只跑一次就 return）。
	// 当资产已在内存中而回调被同步触发时，bBatchCompletedSync 被 OnAssetLoaded 置 true，
	// 循环在原调用栈内续接下一批，消除潜在递归。
	while (true)
	{
		if (Status == EBatchStatus::Stop)
		{
			OnStop();
			return;
		}

		Status = EBatchStatus::Processing;

		TArray<FSoftObjectPath> PendingArray;
		const int32 Count = Context->GetPendingArray(PendingArray, Config->BatchSize);
		if (Count <= 0)
		{
			OnFinish();
			return;
		}

		bBatchCompletedSync = false;
		bAwaitingLoad = true;
		StreamableManager.RequestAsyncLoad(PendingArray,
			FStreamableDelegate::CreateUObject(this, &UBatchRunner::OnAssetLoaded, PendingArray));
		bAwaitingLoad = false;

		if (!bBatchCompletedSync)
		{
			// 正常异步路径：等后续 tick 回调续接，本次返回
			return;
		}
		// 同步完成路径：不递归，继续循环发起下一批
	}
}

void UBatchRunner::OnAssetLoaded(TArray<FSoftObjectPath> PendingArray)
{
	for (auto& TargetPath : PendingArray)
	{
		// 批内响应停止请求：不再处理剩余资产，交由 OnProcessing 走 OnStop 收尾
		if (Status == EBatchStatus::Stop)
		{
			break;
		}

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
				// 蓝图资产：先传播默认值变更再落盘，避免子蓝图 / 已有实例在下次编译时回退。
				// SkipGarbageCollection + BatchCompile：跳过逐次 GC、标记为批量模式；
				// 所有蓝图编译完成后在 FinalizeProcessors 统一执行一次 CollectGarbage。
				if (UBlueprint* BP = Target.GetBlueprint())
				{
					FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
					FKismetEditorUtilities::CompileBlueprint(BP,
						EBlueprintCompileOptions::SkipGarbageCollection | EBlueprintCompileOptions::BatchCompile);
					bCompiledAnyBlueprint = true;
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

	if (bAwaitingLoad)
	{
		// 同步完成路径：RequestAsyncLoad 尚未返回，通知 OnProcessing 循环在原栈内续接
		bBatchCompletedSync = true;
		return;
	}

	// 正常异步路径：OnProcessing 已经返回，在此续接下一批或收尾
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
	FinalizeProcessors();

	if (Config)
	{
		Config->OnRunnerFinished(this, true);
	}
}

void UBatchRunner::FinalizeProcessors()
{
	if (!Config || !Context) return;

	for (const UProcessorBase* Processor : Config->Processors)
	{
		// Instanced 数组可能含 null（编辑器删除内联实例 / 类丢失），跳过以防崩溃
		if (!IsValid(Processor)) continue;
		Processor->Finish(Context);
	}

	// 本轮有蓝图以 BatchCompile 模式编译（跳过了逐次 GC），统一在此执行一次 GC
	if (bCompiledAnyBlueprint)
	{
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		bCompiledAnyBlueprint = false;
	}
}

void UBatchRunner::PreviewMatchedAssets(UBatchBase* Config, TArray<FAssetData>& OutAssets)
{
	if (!IsValid(Config)) return;

	// 只走 pre-load 链路：收集 Scanner 候选集，零加载、零副作用
	TSet<FAssetData> Assets;
	for (const UScannerBase* Scanner : Config->GetScanners())
	{
		if (!IsValid(Scanner)) continue;
		Scanner->ScannerAssets(Assets);
	}

	OutAssets.Reserve(Assets.Num());
	for (const FAssetData& AssetData : Assets)
	{
		OutAssets.Add(AssetData);
	}
}
