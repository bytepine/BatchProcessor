// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Bool.h"

#include "BatchDefine.h"

bool UConditionProperty_Bool::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	FBatchProperty FoundProperty;
	FindProperty(Variable, FoundProperty);
	if (!FoundProperty.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBool: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是布尔类型
	const FBoolProperty* BoolProperty = CastField<FBoolProperty>(FoundProperty.Property);
	if (!BoolProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBool: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

	// 使用 GetPropertyValue 读取布尔值，正确处理位域（bitfield）布尔属性
	const bool bPropertyValue = BoolProperty->GetPropertyValue(BoolProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));

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
