// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Bool.h"

#include "BatchProcessor.h"

bool UConditionProperty_Bool::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = Super::OnCheckCondition(Pointer, Struct);

	void* TargetPoint = nullptr;
	FProperty* TargetProperty = nullptr;
	FindProperty(Pointer, Struct, TargetPoint, TargetProperty);
	if (!TargetPoint || !TargetProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBool: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是布尔类型
	const FBoolProperty* BoolProperty = CastField<FBoolProperty>(TargetProperty);
	if (!BoolProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBool: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

	// 获取布尔值 - 使用标准方法获取
	const bool bPropertyValue = *BoolProperty->ContainerPtrToValuePtr<bool>(TargetPoint);

	switch (ComparisonOperator)
	{
	case EBoolComparisonOperators::Equal:
		bResult = bPropertyValue == bValue;
		break;
	case EBoolComparisonOperators::NotEqual:
		bResult = bPropertyValue != bValue;
		break;
	}
	
	return bResult;
}
