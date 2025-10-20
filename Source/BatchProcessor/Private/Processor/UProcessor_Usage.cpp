// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "UProcessor_Usage.h"

#include "BatchProcessor.h"

void UUProcessor_Usage::OnStart() const
{
	Super::OnStart();

	// BlueprintSet.Empty();
}

bool UUProcessor_Usage::OnProcessing(UBlueprint* Blueprint, void* Pointer, const UStruct* Struct) const
{
	// if (IsValid(Blueprint))
	// {
	// 	if (const FString PathName = Blueprint->GetPathName(); !BlueprintSet.Contains(PathName))
	// 	{
	// 		BlueprintSet.Add(PathName);
	// 	}
	// }
	
	return Super::OnProcessing(Blueprint, Pointer, Struct);
}

void UUProcessor_Usage::OnFinish() const
{
	Super::OnFinish();

	// FString Lines;
	// for (const FString Blueprint : BlueprintSet)
	// {
	// 	UE_LOG(LogBatchProcessor, Log, TEXT("BlueprintUsage: %s"), *Blueprint);
	// 	
	// 	Lines += FString::Printf(TEXT("%s\n"), *Blueprint);
	// }
	//
	// // 创建多行文本对话框
	// const FText DialogTitle = FText::FromString(TEXT("使用结果"));
	// FText DialogText = FText::FromString(TEXT("无"));
	// if (!Lines.IsEmpty())
	// {
	// 	DialogText = FText::FromString(Lines);
	// }
	// FMessageDialog::Open(EAppMsgType::Ok, DialogText, DialogTitle);
	//
	// BlueprintSet.Empty();
}
