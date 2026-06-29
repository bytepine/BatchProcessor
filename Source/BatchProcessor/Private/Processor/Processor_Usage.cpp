// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Usage.h"

#include "BatchContext.h"
#include "BatchDefine.h"

void UProcessor_Usage::OnFinish(UBatchContext* Context) const
{
	Super::OnFinish(Context);

	const FBatchResult& Result = Context->GetResult();

	FString Lines;
	for (const FString& Path : Result.TouchedAssets)
	{
		UE_LOG(LogBatchProcessor, Log, TEXT("ProcessedAsset: %s"), *Path);
		Lines += FString::Printf(TEXT("%s\n"), *Path);
	}

	// 创建多行文本对话框
	const FText DialogTitle = FText::FromString(FString::Printf(TEXT("使用结果 (%s)"), *Result.GetSummary()));
	FText DialogText = FText::FromString(TEXT("无"));
	if (!Lines.IsEmpty())
	{
		DialogText = FText::FromString(Lines);
	}
	FMessageDialog::Open(EAppMsgType::Ok, DialogText, DialogTitle);
}
