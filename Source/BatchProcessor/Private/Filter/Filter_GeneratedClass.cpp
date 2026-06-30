// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Filter_GeneratedClass.h"

UFilter_GeneratedClass::UFilter_GeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

bool UFilter_GeneratedClass::OnShouldKeep(const FBatchTarget& Target) const
{
	// 未配置比较类时视为「配置错误」，排除所有资产并发出警告
	// 避免「漏配 = 静默放行全部资产」的危险默认行为
	if (!GeneratedClass.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning,
			TEXT("Filter_GeneratedClass: GeneratedClass is not set — excluding all assets to prevent unintended batch processing"));
		return false;
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
