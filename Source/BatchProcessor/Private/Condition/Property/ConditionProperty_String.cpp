// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_String.h"

#include "BatchDefine.h"

bool UConditionProperty_String::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	FBatchProperty FoundProperty;
	FindProperty(Variable, FoundProperty);
	if (!FoundProperty.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是字符串类型(FString/FName/FText)
	FString PropertyValue;
	
	const FStrProperty* StrProperty = CastField<FStrProperty>(FoundProperty.Property);
	const FNameProperty* NameProperty = CastField<FNameProperty>(FoundProperty.Property);
	const FTextProperty* TextProperty = CastField<FTextProperty>(FoundProperty.Property);
	
	if (StrProperty)
	{
		PropertyValue = *StrProperty->ContainerPtrToValuePtr<FString>(FoundProperty.Address);
	}
	else if (NameProperty)
	{
		PropertyValue = NameProperty->ContainerPtrToValuePtr<FName>(FoundProperty.Address)->ToString();
	}
	else if (TextProperty)
	{
		PropertyValue = TextProperty->ContainerPtrToValuePtr<FText>(FoundProperty.Address)->ToString();
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
