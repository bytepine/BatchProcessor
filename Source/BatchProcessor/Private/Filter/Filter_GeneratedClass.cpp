// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Filter_GeneratedClass.h"

UFilter_GeneratedClass::UFilter_GeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

bool UFilter_GeneratedClass::OnShouldKeep(const FBatchTarget& Target) const
{
	bool bResult = Super::OnShouldKeep(Target);

	if (Target.IsValid() && GeneratedClass.IsValid())
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
