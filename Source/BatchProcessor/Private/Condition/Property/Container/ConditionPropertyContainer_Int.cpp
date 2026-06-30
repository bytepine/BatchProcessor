// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionPropertyContainer_Int.h"

#include "BatchDefine.h"

bool UConditionPropertyContainer_Int::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	FBatchProperty FoundProperty;
	FindProperty(Variable, FoundProperty);
	if (!FoundProperty.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckIntContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 判断 FNumericProperty 是否为无符号整数类型（FNumericProperty 无 IsUnsignedInteger API）
	auto IsUnsignedIntProp = [](const FNumericProperty* Prop) -> bool
	{
		return CastField<FByteProperty>(Prop) != nullptr
			|| CastField<FUInt16Property>(Prop) != nullptr
			|| CastField<FUInt32Property>(Prop) != nullptr
			|| CastField<FUInt64Property>(Prop) != nullptr;
	};

	// 检查属性是否是整数数组类型
	TArray<int64> IntArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(ArrayProperty->Inner))
		{
			if (NumericProperty->IsInteger())
			{
				const bool bUnsigned = IsUnsignedIntProp(NumericProperty);
				FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
				for (int32 i = 0; i < ArrayHelper.Num(); ++i)
				{
					const void* ElementPtr = ArrayHelper.GetRawPtr(i);
					const int64 ElementValue = bUnsigned
						? static_cast<int64>(NumericProperty->GetUnsignedIntPropertyValue(ElementPtr))
						: NumericProperty->GetSignedIntPropertyValue(ElementPtr);

					IntArray.Add(ElementValue);
				}
			}
			else
			{
				UE_LOG(LogBatchProcessor, Warning, TEXT("CheckIntArray: 数据类型错误 [%s]"), *PropertyName);
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(SetProperty->ElementProp))
		{
			if (NumericProperty->IsInteger())
			{
				const bool bUnsigned = IsUnsignedIntProp(NumericProperty);
				FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
				for (int32 i = 0; i < SetHelper.Num(); ++i)
				{
					if (SetHelper.IsValidIndex(i))
					{
						const void* ElementPtr = SetHelper.GetElementPtr(i);
						const int64 ElementValue = bUnsigned
							? static_cast<int64>(NumericProperty->GetUnsignedIntPropertyValue(ElementPtr))
							: NumericProperty->GetSignedIntPropertyValue(ElementPtr);

						IntArray.Add(ElementValue);
					}
				}
			}
			else
			{
				UE_LOG(LogBatchProcessor, Warning, TEXT("CheckInt64Array: 数据类型错误 [%s]"), *PropertyName);
			}
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(MapProperty->ValueProp))
		{
			if (NumericProperty->IsInteger())
			{
				const bool bUnsigned = IsUnsignedIntProp(NumericProperty);
				FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
				for (int32 i = 0; i < MapHelper.Num(); ++i)
				{
					if (MapHelper.IsValidIndex(i))
					{
						const void* ElementPtr = MapHelper.GetValuePtr(i);
						const int64 ElementValue = bUnsigned
							? static_cast<int64>(NumericProperty->GetUnsignedIntPropertyValue(ElementPtr))
							: NumericProperty->GetSignedIntPropertyValue(ElementPtr);

						IntArray.Add(ElementValue);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckIntContainer: 属性类型错误 [%s]"), *PropertyName);
	}

	bResult |= CheckIntArray(IntArray);
	
	return bResult;
}

bool UConditionPropertyContainer_Int::CheckIntArray(const TArray<int64>& IntArray)
{
	bool bResult = false;

	switch (ComparisonOperator)
	{
	case EBoolContainerComparisonOperators::Include:
		{
			// 多重性语义：Values 中每个值（含重复）都必须在 IntArray 中找到不同位置的匹配
			TArray<int64> Remaining = IntArray;
			bResult = true;
			for (const int64& Value : Values)
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
			// 多重性语义：IntArray 中每个值（含重复）都必须在 Values 中找到不同位置的匹配
			TArray<int64> RemainingValues = Values;
			bResult = true;
			for (const int64& ArrayValue : IntArray)
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
			// Set/Map 迭代顺序不确定，排序后再比较
			if (IntArray.Num() == Values.Num())
			{
				TArray<int64> SortedArray = IntArray;
				TArray<int64> SortedValues = Values;
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
