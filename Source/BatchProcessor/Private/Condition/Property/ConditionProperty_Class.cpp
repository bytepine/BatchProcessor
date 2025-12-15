// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Class.h"

#include "BatchDefine.h"
#include "BatchProcessor.h"

UConditionProperty_Class::UConditionProperty_Class(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

bool UConditionProperty_Class::OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Assets, Context, Variable);

	const UClass* PropertyClass = nullptr;
	if (PropertyName.IsEmpty())
	{
		PropertyClass = Cast<UClass>(Variable.Struct);
	}
	else
	{
		FBatchProperty Target;
		FindProperty(Variable, Target);
		if (!Target.IsValid())
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
			return bResult;
		}

		// 检查属性是否是类属性或对象属性
		const FClassProperty* ClassProperty = CastField<FClassProperty>(Target.Property);
		const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Target.Property);
		const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Target.Property);
		if (!ClassProperty && !SoftClassProperty && !ObjectProperty)
		{
			UE_LOG(LogBatchProcessor, Error, TEXT("CheckClass: 属性类型错误 [%s]"), *PropertyName);
			return bResult;
		}
	
		if (ClassProperty)
		{
			PropertyClass = *ClassProperty->ContainerPtrToValuePtr<UClass*>(Target.Address);
		}
		else if (SoftClassProperty)
		{
			const TSoftClassPtr<>& SoftClassPtr = *SoftClassProperty->ContainerPtrToValuePtr<TSoftClassPtr<>>(Target.Address);
			PropertyClass = SoftClassPtr.Get();
		}
		else if (ObjectProperty)
		{
			// 如果是对象属性，获取对象的类
			const UObject* PropertyObject = *ObjectProperty->ContainerPtrToValuePtr<UObject*>(Target.Address);
			PropertyClass = PropertyObject ? PropertyObject->GetClass() : nullptr;
		}
	}

	// 获取比较的类指针
	const UClass* CompareClass = Value.Get();

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

	return bResult;
}
