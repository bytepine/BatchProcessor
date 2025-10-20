// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_String.h"
#include "BatchDefine.h"
#include "BatchProcessor.h"

bool UConditionProperty_String::OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Context, Variable);

	FBatchProperty Target;
	FindProperty(Variable, Target);
	if (!Target.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是字符串类型(FString/FName/FText)
	FString PropertyValue;
	
	const FStrProperty* StrProperty = CastField<FStrProperty>(Target.Property);
	const FNameProperty* NameProperty = CastField<FNameProperty>(Target.Property);
	const FTextProperty* TextProperty = CastField<FTextProperty>(Target.Property);
	
	if (StrProperty)
	{
		PropertyValue = *StrProperty->ContainerPtrToValuePtr<FString>(Target.Address);
	}
	else if (NameProperty)
	{
		PropertyValue = NameProperty->ContainerPtrToValuePtr<FName>(Target.Address)->ToString();
	}
	else if (TextProperty)
	{
		PropertyValue = TextProperty->ContainerPtrToValuePtr<FText>(Target.Address)->ToString();
	}
	else
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("CheckString: 属性类型错误 [%s]"), *PropertyName);
		return bResult;
	}

	switch (ComparisonOperator)
	{
	case EStringComparisonOperators::Equal:
		bResult = PropertyValue.Equals(Value, ESearchCase::CaseSensitive);
		break;
	case EStringComparisonOperators::NotEqual:
		bResult = !PropertyValue.Equals(Value, ESearchCase::CaseSensitive);
		break;
	case EStringComparisonOperators::Contains:
		bResult = PropertyValue.Contains(Value, ESearchCase::CaseSensitive);
		break;
	case EStringComparisonOperators::NotContains:
		bResult = !PropertyValue.Contains(Value, ESearchCase::CaseSensitive);
		break;
	case EStringComparisonOperators::StartsWith:
		bResult = PropertyValue.StartsWith(Value, ESearchCase::CaseSensitive);
		break;
	case EStringComparisonOperators::EndsWith:
		bResult = PropertyValue.EndsWith(Value, ESearchCase::CaseSensitive);
		break;
	}
	
	return bResult;
}
