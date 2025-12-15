// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Filter_GeneratedClass.h"

UFilter_GeneratedClass::UFilter_GeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

bool UFilter_GeneratedClass::OnFilter(const UBlueprint* Blueprint) const
{
	bool bResult = Super::OnFilter(Blueprint);

	if (IsValid(Blueprint) && GeneratedClass.IsValid())
	{
		// 蓝图生成类
		const UClass* PropertyClass = Blueprint->GeneratedClass.Get();
		
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
	
	return !bResult;
}
