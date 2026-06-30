// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchBase.h"

#include "BatchAssetSaver.h"
#include "BatchRunner.h"

UBatchBase::UBatchBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UBatchBase::Start()
{
	// 已有运行实例在执行时，忽略重复启动
	if (ActiveRunner && ActiveRunner->IsRunning()) return;

	ActiveRunner = NewObject<UBatchRunner>(this);

	// 试运行模式：注入只记录不落盘的保存器
	if (bDryRun)
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
		ActiveRunner = nullptr;
	}

	OnBatchFinished.Broadcast(bSuccess);
}
