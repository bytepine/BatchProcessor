// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ConditionPropertyContainer_Float.h"

#include "BatchDefine.h"
#include "Utils/BatchVersionCompat.h"

bool UConditionPropertyContainer_Float::OnCheckCondition(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Target, Context, Variable);

	FBatchProperty FoundProperty;
	FindProperty(Variable, FoundProperty);
	if (!FoundProperty.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是整数数组类型
	TArray<double> DoubleArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const double ElementValue = NumericProperty->GetFloatingPointPropertyValue(ElementPtr);

				DoubleArray.Add(ElementValue);
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatArray: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			for (int32 i = 0; i < SetHelper.Num(); ++i)
			{
				if (SetHelper.IsValidIndex(i))
				{
					const void* ElementPtr = SetHelper.GetElementPtr(i);
					const double ElementValue = NumericProperty->GetFloatingPointPropertyValue(ElementPtr);

					DoubleArray.Add(ElementValue);
				}
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatSet: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(FoundProperty.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(FoundProperty.Address));
			for (int32 i = 0; i < MapHelper.Num(); ++i)
			{
				if (MapHelper.IsValidIndex(i))
				{
					const void* ElementPtr = MapHelper.GetValuePtr(i);
					const double ElementValue = NumericProperty->GetFloatingPointPropertyValue(ElementPtr);

					DoubleArray.Add(ElementValue);
				}
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatMap: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatContainer: 属性类型错误 [%s]"), *PropertyName);
	}

	bResult |= CheckFloatArray(DoubleArray);
	
	return bResult;
}

bool UConditionPropertyContainer_Float::CheckFloatArray(const TArray<double>& FloatArray)
{
	bool bResult = false;
	constexpr double Tolerance = 1e-6;

	switch (ComparisonOperator)
	{
	case EBoolContainerComparisonOperators::Include:
		{
			// 多重性语义：Values 中每个值（含重复）消耗 FloatArray 中一个匹配位置
			TArray<double> Remaining = FloatArray;
			bResult = true;
			for (const double& Value : Values)
			{
				bool bFound = false;
				for (int32 j = 0; j < Remaining.Num(); ++j)
				{
					if (FMath::Abs(Remaining[j] - Value) < Tolerance)
					{
						Remaining.RemoveAt(j, 1, BP_ALLOW_SHRINKING_NO);
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					bResult = false;
					break;
				}
			}
		}
		break;
	case EBoolContainerComparisonOperators::Included:
		{
			// 多重性语义：FloatArray 中每个值（含重复）消耗 Values 中一个匹配位置
			TArray<double> RemainingValues = Values;
			bResult = true;
			for (const double& ArrayValue : FloatArray)
			{
				bool bFound = false;
				for (int32 j = 0; j < RemainingValues.Num(); ++j)
				{
					if (FMath::Abs(RemainingValues[j] - ArrayValue) < Tolerance)
					{
						RemainingValues.RemoveAt(j, 1, BP_ALLOW_SHRINKING_NO);
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					bResult = false;
					break;
				}
			}
		}
		break;
	case EBoolContainerComparisonOperators::Equal:
		{
			// Set/Map 迭代顺序不确定，排序后再按容差逐元素比较
			if (FloatArray.Num() == Values.Num())
			{
				TArray<double> SortedArray = FloatArray;
				TArray<double> SortedValues = Values;
				SortedArray.Sort();
				SortedValues.Sort();
				bResult = true;
				for (int32 i = 0; i < SortedArray.Num(); ++i)
				{
					if (FMath::Abs(SortedArray[i] - SortedValues[i]) >= Tolerance)
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
