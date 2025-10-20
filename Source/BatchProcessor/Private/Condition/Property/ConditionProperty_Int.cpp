// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Int.h"

#include "BatchDefine.h"
#include "BatchProcessor.h"

bool UConditionProperty_Int::OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Context, Variable);

	FBatchProperty Target;
	FindProperty(Variable, Target);
	if (!Target.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 检查属性是否是整数类型
	const FNumericProperty* NumericProperty = CastField<FNumericProperty>(Target.Property);
	if (!NumericProperty || !NumericProperty->IsInteger())
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("CheckInt: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

		// 获取整数值(支持所有整数类型)
	int64 PropertyValue = 0;
	// 直接获取整数值
	// 对于整数类型，可以直接使用GetSignedIntPropertyValue/GetUnsignedIntPropertyValue
	if (const FInt8Property* Int8Prop = CastField<FInt8Property>(NumericProperty))
	{
		PropertyValue = *Int8Prop->ContainerPtrToValuePtr<int8>(Target.Address);
	}
	else if (const FInt16Property* Int16Prop = CastField<FInt16Property>(NumericProperty))
	{
		PropertyValue = *Int16Prop->ContainerPtrToValuePtr<int16>(Target.Address);
	}
	else if (const FIntProperty* Int32Prop = CastField<FIntProperty>(NumericProperty))
	{
		PropertyValue = *Int32Prop->ContainerPtrToValuePtr<int32>(Target.Address);
	}
	else if (const FInt64Property* Int64Prop = CastField<FInt64Property>(NumericProperty))
	{
		PropertyValue = *Int64Prop->ContainerPtrToValuePtr<int64>(Target.Address);
	}
	else if (const FByteProperty* ByteProp = CastField<FByteProperty>(NumericProperty))
	{
		PropertyValue = *ByteProp->ContainerPtrToValuePtr<uint8>(Target.Address);
	}
	else if (const FUInt16Property* UInt16Prop = CastField<FUInt16Property>(NumericProperty))
	{
		PropertyValue = *UInt16Prop->ContainerPtrToValuePtr<uint16>(Target.Address);
	}
	else if (const FUInt32Property* UInt32Prop = CastField<FUInt32Property>(NumericProperty))
	{
		PropertyValue = *UInt32Prop->ContainerPtrToValuePtr<uint32>(Target.Address);
	}
	else if (const FUInt64Property* UInt64Prop = CastField<FUInt64Property>(NumericProperty))
	{
		PropertyValue = *UInt64Prop->ContainerPtrToValuePtr<uint64>(Target.Address);
	}

	switch (ComparisonOperator)
	{
	case EIntComparisonOperators::Equal:
		bResult = PropertyValue == Value;
		break;
	case EIntComparisonOperators::NotEqual:
		bResult = PropertyValue != Value;
		break;
	case EIntComparisonOperators::Greater:
		bResult = PropertyValue > Value;
		break;
	case EIntComparisonOperators::Less:
		bResult = PropertyValue < Value;
		break;
	case EIntComparisonOperators::GreaterOrEqual:
		bResult = PropertyValue >= Value;
		break;
	case EIntComparisonOperators::LessOrEqual:
		bResult = PropertyValue <= Value;
		break;
	}
	
	return bResult;
}
