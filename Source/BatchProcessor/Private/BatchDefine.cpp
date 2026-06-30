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
	if (UBlueprint* BP = GetBlueprint())
	{
		// 蓝图资产：变量根为 GeneratedClass 的 CDO（属性默认值存储在 CDO）
		if (UClass* Class = BP->GeneratedClass.Get())
		{
			return Class->GetDefaultObject();
		}
		return nullptr;
	}

	// 非蓝图资产（DataAsset / Material 等）：变量根为资产实例本身
	// 注意：不能用 Asset->GetClass()->GetDefaultObject()，否则改的是类 CDO
	// 而 SaveAsset 保存的是资产包，两者不同，改动不会持久化。
	return Asset;
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
