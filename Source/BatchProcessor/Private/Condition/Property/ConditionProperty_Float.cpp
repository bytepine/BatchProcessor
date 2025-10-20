﻿// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Float.h"

#include "BatchProcessor.h"

bool UConditionProperty_Float::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = Super::OnCheckCondition(Pointer, Struct);

	void* TargetPoint = nullptr;
	FProperty* TargetProperty = nullptr;
	FindProperty(Pointer, Struct, TargetPoint, TargetProperty);
	if (!TargetPoint || !TargetProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloat: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是浮点数类型
	const FNumericProperty* NumericProperty = CastField<FNumericProperty>(TargetProperty);
	if (!NumericProperty || !NumericProperty->IsFloatingPoint())
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("CheckFloat: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

	// 获取浮点数值
	double PropertyValue = 0.0;
	if (const FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(NumericProperty))
	{
		PropertyValue = *DoubleProp->ContainerPtrToValuePtr<double>(TargetPoint);
	}
	else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(NumericProperty))
	{
		PropertyValue = *FloatProp->ContainerPtrToValuePtr<float>(TargetPoint);
	}

	// 使用更精确的double比较容差
	const double Tolerance = FMath::Max(FMath::Abs(PropertyValue), FMath::Abs(Value)) * DBL_EPSILON * 10;

	switch (ComparisonOperator)
	{
	case EFloatComparisonOperators::Equal:
		bResult = FMath::Abs(PropertyValue - Value) <= Tolerance;
		break;
	case EFloatComparisonOperators::NotEqual:
		bResult = FMath::Abs(PropertyValue - Value) > Tolerance;
		break;
	case EFloatComparisonOperators::Greater:
		bResult = PropertyValue > Value + Tolerance;
		break;
	case EFloatComparisonOperators::Less:
		bResult = PropertyValue < Value - Tolerance;
		break;
	case EFloatComparisonOperators::GreaterOrEqual:
		bResult = PropertyValue >= Value - Tolerance;
		break;
	case EFloatComparisonOperators::LessOrEqual:
		bResult = PropertyValue <= Value + Tolerance;
		break;
	}
	
	return bResult;
}
