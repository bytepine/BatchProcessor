// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "UProcessor_Usage.h"

#include "BatchContext.h"
#include "BatchProcessor.h"

void UUProcessor_Usage::OnStart(UBatchContext* Context) const
{
	Super::OnStart(Context);
}

bool UUProcessor_Usage::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	if (IsValid(Assets))
	{
		if (UBatchScratchPad_Usage* ScratchPad = Context->GetScratchPad<UBatchScratchPad_Usage>(this))
		{
			if (const FString PathName = Assets->GetPathName(); !ScratchPad->AssetsSet.Contains(PathName))
			{
				ScratchPad->AssetsSet.Add(PathName);
			}
		}
	}
	
	return Super::OnProcessing(Assets, Context, Variable);
}

void UUProcessor_Usage::OnFinish(UBatchContext* Context) const
{
	Super::OnFinish(Context);

	FString Lines;
	if (UBatchScratchPad_Usage* ScratchPad = Context->GetScratchPad<UBatchScratchPad_Usage>(this))
	{
		for (const FString Ability : ScratchPad->AssetsSet)
		{
			UE_LOG(LogBatchProcessor, Log, TEXT("BlueprintUsage: %s"), *Ability);
		
			Lines += FString::Printf(TEXT("%s\n"), *Ability);
		}
	}
	
	// 创建多行文本对话框
	const FText DialogTitle = FText::FromString(TEXT("使用结果"));
	FText DialogText = FText::FromString(TEXT("无"));
	if (!Lines.IsEmpty())
	{
		DialogText = FText::FromString(Lines);
	}
	FMessageDialog::Open(EAppMsgType::Ok, DialogText, DialogTitle);
}

UClass* UUProcessor_Usage::GetScratchPadClass() const
{
	return UBatchScratchPad_Usage::StaticClass();
}
