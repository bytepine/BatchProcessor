// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchDefine.h"

#include "Engine/Blueprint.h"

bool FBatchTarget::IsValid() const
{
	return ::IsValid(Asset);
}

UBlueprint* FBatchTarget::GetBlueprint() const
{
	return Cast<UBlueprint>(Asset);
}

UClass* FBatchTarget::GetGeneratedClass() const
{
	if (UBlueprint* BP = GetBlueprint())
	{
		return BP->GeneratedClass.Get();
	}
	return Asset ? Asset->GetClass() : nullptr;
}

UObject* FBatchTarget::GetVariableObject() const
{
	if (UClass* Class = GetGeneratedClass())
	{
		return Class->GetDefaultObject();
	}
	return nullptr;
}

FBatchVariable FBatchTarget::MakeVariable() const
{
	if (UObject* VarObj = GetVariableObject())
	{
		return FBatchVariable(VarObj);
	}
	return FBatchVariable();
}

FString FBatchTarget::GetName() const
{
	return Asset ? Asset->GetName() : FString();
}

FString FBatchTarget::GetPathName() const
{
	return Asset ? Asset->GetPathName() : FString();
}

FString FBatchResult::GetSummary(bool bDryRun) const
{
	const TCHAR* ModifiedLabel = bDryRun ? TEXT("会修改") : TEXT("修改");
	return FString::Printf(TEXT("处理 %d | %s %d | 跳过 %d | 失败 %d"),
		ProcessedCount, ModifiedLabel, ModifiedCount, SkippedCount, FailedCount);
}
