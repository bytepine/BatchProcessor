// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Bool.h"

#include "BatchDefine.h"
#include "BatchProcessor.h"

bool UConditionProperty_Bool::OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Context, Variable);

	FBatchProperty Target;
	FindProperty(Variable, Target);
	if (!Target.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是布尔类型
	const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Target.Property);
	if (!BoolProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBool: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

	// 获取布尔值 - 使用标准方法获取
	const bool bPropertyValue = *BoolProperty->ContainerPtrToValuePtr<bool>(Target.Address);

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
