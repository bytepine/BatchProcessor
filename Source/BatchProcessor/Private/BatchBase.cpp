// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

#include "BatchAssetSaver.h"
#include "BatchContext.h"
#include "BatchProgressReporter.h"
#include "BatchRunner.h"
#include "FilterBase.h"
#include "ProcessorBase.h"
#include "ScannerBase.h"

UBatchBase::UBatchBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UBatchBase::Start()
{
	StartWithReporter(nullptr);
}

void UBatchBase::StartWithReporter(TSharedPtr<IBatchProgressReporter> InReporter, bool bForceDryRun)
{
	// 已有运行实例在执行时，忽略重复启动
	if (ActiveRunner && ActiveRunner->IsRunning()) return;

	ActiveRunner = NewObject<UBatchRunner>(this);

	// 注入自定义进度 Reporter（优先），否则 Runner::Run 内默认使用 Slate 通知
	if (InReporter.IsValid())
	{
		ActiveRunner->SetProgressReporter(InReporter.ToSharedRef());
	}

	// 试运行模式：注入只记录不落盘的保存器
	if (bDryRun || bForceDryRun)
	{
		ActiveRunner->SetAssetSaver(MakeShared<FDryRunBatchAssetSaver>());
	}

	ActiveRunner->Run(this);
}

void UBatchBase::Stop()
{
	if (ActiveRunner)
	{
		ActiveRunner->RequestStop();
	}
}

bool UBatchBase::IsRunning() const
{
	return ActiveRunner && ActiveRunner->IsRunning();
}

void UBatchBase::OnRunnerFinished(UBatchRunner* Runner, bool bSuccess)
{
	if (ActiveRunner == Runner)
	{
		// 完成时存档结果快照，供编辑器在 Runner 清空后仍能读取
		if (IsValid(Runner))
		{
			if (const UBatchContext* Ctx = Runner->GetContext())
			{
				LastResult = Ctx->GetResult();
			}
		}
		ActiveRunner = nullptr;
	}

	OnBatchFinished.Broadcast(bSuccess);
}

// ── 组件管理 ────────────────────────────────────────────────────────────────

void UBatchBase::AddScanner(UScannerBase* Scanner)    { if (Scanner) Scanners.Add(Scanner); }
void UBatchBase::RemoveScanner(int32 Index)           { if (Scanners.IsValidIndex(Index)) Scanners.RemoveAt(Index); }
void UBatchBase::MoveScanner(int32 From, int32 To)    { if (Scanners.IsValidIndex(From) && Scanners.IsValidIndex(To)) Scanners.Swap(From, To); }

void UBatchBase::AddFilter(UFilterBase* Filter)       { if (Filter) Filters.Add(Filter); }
void UBatchBase::RemoveFilter(int32 Index)            { if (Filters.IsValidIndex(Index)) Filters.RemoveAt(Index); }
void UBatchBase::MoveFilter(int32 From, int32 To)     { if (Filters.IsValidIndex(From) && Filters.IsValidIndex(To)) Filters.Swap(From, To); }

void UBatchBase::AddProcessor(UProcessorBase* Processor) { if (Processor) Processors.Add(Processor); }
void UBatchBase::RemoveProcessor(int32 Index)            { if (Processors.IsValidIndex(Index)) Processors.RemoveAt(Index); }
void UBatchBase::MoveProcessor(int32 From, int32 To)     { if (Processors.IsValidIndex(From) && Processors.IsValidIndex(To)) Processors.Swap(From, To); }
