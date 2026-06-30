// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Filter_GeneratedClass.h"

UFilter_GeneratedClass::UFilter_GeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

void UFilter_GeneratedClass::ValidateConfig(TArray<FText>& OutErrors) const
{
	if (GeneratedClass.IsNull())
	{
		OutErrors.Add(FText::FromString(
			TEXT("Filter_GeneratedClass: GeneratedClass 未配置 — 运行时将排除全部资产")));
	}
	else if (!GeneratedClass.IsValid())
	{
		OutErrors.Add(FText::Format(
			FText::FromString(TEXT("Filter_GeneratedClass: GeneratedClass [{0}] 已配置但尚未加载")),
			FText::FromString(GeneratedClass.ToString())));
	}
}

bool UFilter_GeneratedClass::OnShouldKeep(const FBatchTarget& Target) const
{
	// 未配置（IsNull）或已配置但类尚未加载（IsValid=false）时，
	// 视为「配置错误」，排除所有资产。
	// 关键：返回 bInvert 而非固定 false，使 ShouldKeep 在 bInvert=true 时
	// 最终结果依然为 false（排除），防止取反绕过安全策略。
	if (GeneratedClass.IsNull())
	{
		UE_LOG(LogBatchProcessor, Warning,
			TEXT("Filter_GeneratedClass: GeneratedClass is not set — excluding all assets to prevent unintended batch processing"));
		return bInvert;
	}
	if (!GeneratedClass.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning,
			TEXT("Filter_GeneratedClass: GeneratedClass [%s] is set but not loaded — excluding all assets. Ensure the class is loaded before running."),
			*GeneratedClass.ToString());
		return bInvert;
	}

	bool bResult = Super::OnShouldKeep(Target);

	if (Target.IsValid())
	{
		// 资产生成类（蓝图取 GeneratedClass，其它资产取自身的类）
		const UClass* PropertyClass = Target.GetGeneratedClass();
		
		// 获取比较的类指针
		const UClass* CompareClass = GeneratedClass.Get();

		// 比较类或对象
		switch (ComparisonOperator)
		{
		case EClassComparisonOperators::Equal:
			bResult = PropertyClass == CompareClass;
			break;
		case EClassComparisonOperators::NotEqual:
			bResult = PropertyClass != CompareClass;
			break;
		case EClassComparisonOperators::Child:
			bResult = PropertyClass && CompareClass && PropertyClass->IsChildOf(CompareClass);
			break;
		case EClassComparisonOperators::Super:
			bResult = PropertyClass && CompareClass && CompareClass->IsChildOf(PropertyClass);
			break;
		}
	}

	return bResult;
}
