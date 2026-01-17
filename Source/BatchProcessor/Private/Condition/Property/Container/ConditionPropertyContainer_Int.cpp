// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionPropertyContainer_Int.h"

#include "BatchDefine.h"

bool UConditionPropertyContainer_Int::OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bResult = Super::OnCheckCondition(Assets, Context, Variable);

	FBatchProperty Target;
	FindProperty(Variable, Target);
	if (!Target.IsValid())
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

		// 检查属性是否是整数数组类型
	TArray<int64> IntArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Target.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(ArrayProperty->Inner))
		{
			if (NumericProperty->IsInteger())
			{
				FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Target.Address));
				for (int32 i = 0; i < ArrayHelper.Num(); ++i)
				{
					const void* ElementPtr = ArrayHelper.GetRawPtr(i);
					const int64 ElementValue = NumericProperty->GetSignedIntPropertyValue(ElementPtr);

					IntArray.Add(ElementValue);
				}
			}
			else
			{
				UE_LOG(LogBatchProcessor, Warning, TEXT("CheckIntArray: 数据类型错误 [%s]"), *PropertyName);
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Target.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(SetProperty->ElementProp))
		{
			if (NumericProperty->IsInteger())
			{
				FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Target.Address));
				for (int32 i = 0; i < SetHelper.Num(); ++i)
				{
					if (SetHelper.IsValidIndex(i))
					{
						const void* ElementPtr = SetHelper.GetElementPtr(i);
						const int64 ElementValue = NumericProperty->GetSignedIntPropertyValue(ElementPtr);

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
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Target.Property))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(MapProperty->ValueProp))
		{
			if (NumericProperty->IsInteger())
			{
				FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Target.Address));
				for (int32 i = 0; i < MapHelper.Num(); ++i)
				{
					if (MapHelper.IsValidIndex(i))
					{
						const void* ElementPtr = MapHelper.GetValuePtr(i);
						const int64 ElementValue = NumericProperty->GetSignedIntPropertyValue(ElementPtr);

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
			if (IntArray.Num() >= Values.Num())
			{
				bResult = true;
				for (const int64& Value : Values)
				{
					if (!IntArray.Contains(Value))
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
			if (Values.Num() >= IntArray.Num())
			{
				bResult = true;
				for (const int64& ArrayValue : IntArray)
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
			if (IntArray.Num() == Values.Num())
			{
				bResult = true;
				for (int32 i = 0; i < IntArray.Num(); ++i)
				{
					if (IntArray[i] != Values[i])
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
