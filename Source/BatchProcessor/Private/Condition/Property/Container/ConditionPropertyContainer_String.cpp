// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionPropertyContainer_String.h"

#include "BatchDefine.h"
#include "BatchProcessor.h"

bool UConditionPropertyContainer_String::OnCheckCondition(UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Context, Variable);

	FBatchProperty Target;
	FindProperty(Variable, Target);
	if (!Target.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 检查属性是否是字符串数组类型
	TArray<FString> StringArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Target.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const FString& ElementValue = StrProperty->GetPropertyValue(ElementPtr);

				StringArray.Add(ElementValue);
			}
		}
		else if (const FTextProperty* TextProperty = CastField<FTextProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const FText& ElementValue = TextProperty->GetPropertyValue(ElementPtr);

				StringArray.Add(ElementValue.ToString());
			}
		}
		else if (const FNameProperty* NameProperty = CastField<FNameProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const FName& ElementValue = NameProperty->GetPropertyValue(ElementPtr);

				StringArray.Add(ElementValue.ToString());
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckStringArray: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Target.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < SetHelper.Num(); ++i)
			{
				if (SetHelper.IsValidIndex(i))
				{
					const void* ElementPtr = SetHelper.GetElementPtr(i);
					const FString& ElementValue = StrProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue);
				}
			}
		}
		else if (const FTextProperty* TextProperty = CastField<FTextProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < SetHelper.Num(); ++i)
			{
				if (SetHelper.IsValidIndex(i))
				{
					const void* ElementPtr = SetHelper.GetElementPtr(i);
					const FText& ElementValue = TextProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue.ToString());
				}
			}
		}
		else if (const FNameProperty* NameProperty = CastField<FNameProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < SetHelper.Num(); ++i)
			{
				if (SetHelper.IsValidIndex(i))
				{
					const void* ElementPtr = SetHelper.GetElementPtr(i);
					const FName& ElementValue = NameProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckStringArray: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Target.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < MapHelper.Num(); ++i)
			{
				if (MapHelper.IsValidIndex(i))
				{
					const void* ElementPtr = MapHelper.GetValuePtr(i);
					const FString& ElementValue = StrProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue);
				}
			}
		}
		else if (const FTextProperty* TextProperty = CastField<FTextProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < MapHelper.Num(); ++i)
			{
				if (MapHelper.IsValidIndex(i))
				{
					const void* ElementPtr = MapHelper.GetValuePtr(i);
					const FText& ElementValue = TextProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue.ToString());
				}
			}
		}
		else if (const FNameProperty* NameProperty = CastField<FNameProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < MapHelper.Num(); ++i)
			{
				if (MapHelper.IsValidIndex(i))
				{
					const void* ElementPtr = MapHelper.GetValuePtr(i);
					const FName& ElementValue = NameProperty->GetPropertyValue(ElementPtr);

					StringArray.Add(ElementValue.ToString());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckStringContainer: 属性类型错误 [%s]"), *PropertyName);
	}

	bResult |= CheckStringArray(StringArray);
	
	return bResult;
}

bool UConditionPropertyContainer_String::CheckStringArray(const TArray<FString>& StringArray)
{
	bool bResult = false;

	switch (ComparisonOperator)
	{
	case EBoolContainerComparisonOperators::Include:
		{
			if (StringArray.Num() >= Values.Num())
			{
				bResult = true;
				for (const FString& Value : Values)
				{
					if (!StringArray.Contains(Value))
					{
						bResult = false;
						break;
					}
				}
			}
		}
		break;
	case EBoolContainerComparisonOperators::Included:
		{
			if (Values.Num() >= StringArray.Num())
			{
				bResult = true;
				for (const FString& ArrayValue : StringArray)
				{
					if (!Values.Contains(ArrayValue))
					{
						bResult = false;
						break;
					}
				}
			}
		}
		break;
	case EBoolContainerComparisonOperators::Equal:
		{
			if (StringArray.Num() == Values.Num())
			{
				bResult = true;
				for (int32 i = 0; i < StringArray.Num(); ++i)
				{
					if (StringArray[i] != Values[i])
					{
						bResult = false;
						break;
					}
				}
			}
		}
		break;
	}

	return bResult;
}
