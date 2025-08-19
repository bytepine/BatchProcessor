// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionProperty_Class.h"

#include "BatchProcessor.h"

bool UConditionProperty_Class::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = Super::OnCheckCondition(Pointer, Struct);

	const UClass* PropertyClass = nullptr;
	if (PropertyName.IsEmpty())
	{
		PropertyClass = Cast<UClass>(Struct);
	}
	else
	{
		void* TargetPoint = nullptr;
		FProperty* TargetProperty = nullptr;
		FindProperty(Pointer, Struct, TargetPoint, TargetProperty);
		if (!TargetPoint || !TargetProperty)
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckClass: 没找到属性 [%s]"), *PropertyName);
			return bResult;
		}

		// 检查属性是否是类属性或对象属性
		const FClassProperty* ClassProperty = CastField<FClassProperty>(TargetProperty);
		const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(TargetProperty);
		const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(TargetProperty);
		if (!ClassProperty && !SoftClassProperty && !ObjectProperty)
		{
			UE_LOG(LogBatchProcessor, Error, TEXT("CheckClass: 属性类型错误 [%s]"), *PropertyName);
			return bResult;
		}
	
		if (ClassProperty)
		{
			PropertyClass = *ClassProperty->ContainerPtrToValuePtr<UClass*>(TargetPoint);
		}
		else if (SoftClassProperty)
		{
			const TSoftClassPtr<>& SoftClassPtr = *SoftClassProperty->ContainerPtrToValuePtr<TSoftClassPtr<>>(TargetPoint);
			PropertyClass = SoftClassPtr.Get();
		}
		else if (ObjectProperty)
		{
			// 如果是对象属性，获取对象的类
			const UObject* PropertyObject = *ObjectProperty->ContainerPtrToValuePtr<UObject*>(TargetPoint);
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
