// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionProperty_Class.h"

#include "BatchDefine.h"

UConditionProperty_Class::UConditionProperty_Class(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ComparisonOperator(EClassComparisonOperators::Child)
{
	
}

bool UConditionProperty_Class::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	const UClass* PropertyClass = nullptr;
	if (PropertyName.IsEmpty())
	{
		PropertyClass = Cast<UClass>(Variable.Struct);
	}
	else
	{
		FBatchProperty FoundProperty;
		FindProperty(Variable, FoundProperty);
		if (!FoundProperty.IsValid())
		{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckClass: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 检查属性是否是类属性或对象属性
		const FClassProperty* ClassProperty = CastField<FClassProperty>(FoundProperty.Property);
		const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(FoundProperty.Property);
		const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(FoundProperty.Property);
		if (!ClassProperty && !SoftClassProperty && !ObjectProperty)
		{
			UE_LOG(LogBatchProcessor, Error, TEXT("CheckClass: 属性类型错误 [%s]"), *PropertyName);
			return bResult;
		}
	
		if (ClassProperty)
		{
			// 使用 GetPropertyValue 而非裸指针解引用，兼容弱引用/代理场景
			PropertyClass = Cast<UClass>(ClassProperty->GetObjectPropertyValue(ClassProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address)));
		}
		else if (SoftClassProperty)
		{
			const TSoftClassPtr<>& SoftClassPtr = *SoftClassProperty->ContainerPtrToValuePtr<TSoftClassPtr<>>(FoundProperty.Address);
			PropertyClass = SoftClassPtr.Get();
		}
		else if (ObjectProperty)
		{
			// 如果是对象属性，获取对象的类
			const UObject* PropertyObject = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			PropertyClass = PropertyObject ? PropertyObject->GetClass() : nullptr;
		}
	}

	// 获取比较的类指针，并对未配置/未加载的情况发出明确警告
	if (Value.IsNull())
	{
		UE_LOG(LogBatchProcessor, Warning,
			TEXT("CheckClass: Value is not set — condition will not match. PropertyName=[%s]"),
			*PropertyName);
		return bResult;
	}
	if (!Value.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning,
			TEXT("CheckClass: Value [%s] is set but not loaded — condition will not match. Ensure the class is loaded before running."),
			*Value.ToString());
		return bResult;
	}
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
