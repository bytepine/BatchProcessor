// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionPropertyContainer_Bool.h"

#include "BatchDefine.h"

bool UConditionPropertyContainer_Bool::OnCheckCondition(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
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
	TArray<bool> BoolArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Target.Property))
	{
		if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < ArrayHelper.Num(); ++i)
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				const bool ElementValue = BoolProperty->GetPropertyValue(ElementPtr);

				BoolArray.Add(ElementValue);
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolArray: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Target.Property))
	{
		if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < SetHelper.Num(); ++i)
			{
				if (SetHelper.IsValidIndex(i))
				{
					const void* ElementPtr = SetHelper.GetElementPtr(i);
					const bool ElementValue = BoolProperty->GetPropertyValue(ElementPtr);

					BoolArray.Add(ElementValue);
				}
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolSet: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Target.Property))
	{
		if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Target.Address));
			for (int32 i = 0; i < MapHelper.Num(); ++i)
			{
				if (MapHelper.IsValidIndex(i))
				{
					const void* ElementPtr = MapHelper.GetValuePtr(i);
					const bool ElementValue = BoolProperty->GetPropertyValue(ElementPtr);

					BoolArray.Add(ElementValue);
				}
			}
		}
		else
		{
			UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolMap: 数据类型错误 [%s]"), *PropertyName);
		}
	}
	else
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckBoolContainer: 属性类型错误 [%s]"), *PropertyName);
	}

	bResult |= CheckBoolArray(BoolArray);
	
	return bResult;
}

bool UConditionPropertyContainer_Bool::CheckBoolArray(const TArray<bool>& BoolArray) const
{
	bool bResult = false;

	int32 ArrayTrueCount = 0;
	int32 ArrayFalseCount = 0;
	for (const bool bValue : BoolArray)
	{
		if (bValue)
		{
			ArrayTrueCount++;
		}
		else
		{
			ArrayFalseCount++;
		}
	}

	int32 ValueTrueCount = 0;
	int32 ValueFalseCount = 0;
	for (const bool bValue : Values)
	{
		if (bValue)
		{
			ValueTrueCount++;
		}
		else
		{
			ValueFalseCount++;
		}
	}
	
	switch (ComparisonOperator)
	{
	case EBoolContainerComparisonOperators::Include:
		{
			if (BoolArray.Num() >= Values.Num() &&
				ArrayTrueCount >= ValueTrueCount &&
				ArrayFalseCount >= ValueFalseCount)
			{
				bResult = true;
			}
		}
		break;
	case EBoolContainerComparisonOperators::Included:
		{
			if (BoolArray.Num() <= Values.Num() &&
				ArrayTrueCount <= ValueTrueCount &&
				ArrayFalseCount <= ValueFalseCount)
			{
				bResult = true;
			}
		}
		break;
	case EBoolContainerComparisonOperators::Equal:
		{
			if (BoolArray.Num() == Values.Num())
			{
				bool bAllMatch = true;
				for (int32 i = 0; i < BoolArray.Num(); ++i)
				{
					if (BoolArray[i] != Values[i])
					{
						bAllMatch = false;
						break;
					}
				}
				bResult = bAllMatch;
			}
		}
		break;
	}
	
	return bResult;
}