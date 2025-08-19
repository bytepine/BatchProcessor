// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionProperty_String.h"

#include "BatchProcessor.h"

bool UConditionProperty_String::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = Super::OnCheckCondition(Pointer, Struct);

	void* TargetPoint = nullptr;
	FProperty* TargetProperty = nullptr;
	FindProperty(Pointer, Struct, TargetPoint, TargetProperty);
	if (!TargetPoint || !TargetProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckString: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是字符串类型(FString/FName/FText)
	FString PropertyValue;
	
	const FStrProperty* StrProperty = CastField<FStrProperty>(TargetProperty);
	const FNameProperty* NameProperty = CastField<FNameProperty>(TargetProperty);
	const FTextProperty* TextProperty = CastField<FTextProperty>(TargetProperty);
	
	if (StrProperty)
	{
		PropertyValue = *StrProperty->ContainerPtrToValuePtr<FString>(TargetPoint);
	}
	else if (NameProperty)
	{
		PropertyValue = NameProperty->ContainerPtrToValuePtr<FName>(TargetPoint)->ToString();
	}
	else if (TextProperty)
	{
		PropertyValue = TextProperty->ContainerPtrToValuePtr<FText>(TargetPoint)->ToString();
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
