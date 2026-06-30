// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionPropertyContainer_String.h"

#include "BatchDefine.h"

bool UConditionPropertyContainer_String::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	FBatchProperty FoundProperty;
	FindProperty(Variable, FoundProperty);
	if (!FoundProperty.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckStringContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 检查属性是否是字符串数组类型
	TArray<FString> StringArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(FoundProperty.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const FString& ElementValue = StrProperty->GetPropertyValue(ElementPtr);

				StringArray.Add(ElementValue);
			}
		}
		else if (const FTextProperty* TextProperty = CastField<FTextProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const FText& ElementValue = TextProperty->GetPropertyValue(ElementPtr);

				StringArray.Add(ElementValue.ToString());
			}
		}
		else if (const FNameProperty* NameProperty = CastField<FNameProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(FoundProperty.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(FoundProperty.Property))
	{
		if (const FStrProperty* StrProperty = CastField<FStrProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
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
			// 多重性语义：Values 中每个值（含重复）消耗 StringArray 中一个匹配位置
			TArray<FString> Remaining = StringArray;
			bResult = true;
			for (const FString& Value : Values)
			{
				const int32 Idx = Remaining.Find(Value);
				if (Idx == INDEX_NONE)
				{
					bResult = false;
					break;
				}
				Remaining.RemoveAt(Idx, 1, EAllowShrinking::No);
			}
		}
		break;
	case EBoolContainerComparisonOperators::Included:
		{
			// 多重性语义：StringArray 中每个值（含重复）消耗 Values 中一个匹配位置
			TArray<FString> RemainingValues = Values;
			bResult = true;
			for (const FString& ArrayValue : StringArray)
			{
				const int32 Idx = RemainingValues.Find(ArrayValue);
				if (Idx == INDEX_NONE)
				{
					bResult = false;
					break;
				}
				RemainingValues.RemoveAt(Idx, 1, EAllowShrinking::No);
			}
		}
		break;
	case EBoolContainerComparisonOperators::Equal:
		{
			// Set/Map 迭代顺序不确定，排序后再逐元素比较
			if (StringArray.Num() == Values.Num())
			{
				TArray<FString> SortedArray = StringArray;
				TArray<FString> SortedValues = Values;
				SortedArray.Sort();
				SortedValues.Sort();
				bResult = true;
				for (int32 i = 0; i < SortedArray.Num(); ++i)
				{
					if (SortedArray[i] != SortedValues[i])
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
