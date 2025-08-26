// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionPropertyContainer_Float.h"

#include "BatchProcessor.h"

bool UConditionPropertyContainer_Float::OnCheckCondition(void* Pointer, const UStruct* Struct)
{
	bool bResult = Super::OnCheckCondition(Pointer, Struct);

	void* TargetPoint = nullptr;
	FProperty* TargetProperty = nullptr;
	FindProperty(Pointer, Struct, TargetPoint, TargetProperty);
	if (!TargetPoint || !TargetProperty)
	{
		UE_LOG(LogBatchProcessor, Warning, TEXT("CheckFloatContainer: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}

	// 检查属性是否是整数数组类型
	TArray<double> DoubleArray;
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(TargetProperty))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(ArrayProperty->Inner))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(TargetPoint));
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
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(TargetProperty))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(SetProperty->ElementProp))
		{
			FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(TargetPoint));
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
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(TargetProperty))
	{
		if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(MapProperty->ValueProp))
		{
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(TargetPoint));
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
			if (FloatArray.Num() >= Values.Num())
			{
				bResult = true;
				for (const double& Value : Values)
				{
					bool bFound = false;
					for (const double& ArrayValue : FloatArray)
					{
						if (FMath::Abs(ArrayValue - Value) < Tolerance)
						{
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
		}
		break;
	case EBoolContainerComparisonOperators::Included:
		{
			if (Values.Num() >= FloatArray.Num())
			{
				bResult = true;
				for (const double& ArrayValue : FloatArray)
				{
					bool bFound = false;
					for (const double& Value : Values)
					{
						if (FMath::Abs(ArrayValue - Value) < Tolerance)
						{
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
		}
		break;
	case EBoolContainerComparisonOperators::Equal:
		{
			if (FloatArray.Num() == Values.Num())
			{
				bResult = true;
				for (int32 i = 0; i < FloatArray.Num(); ++i)
				{
					if (FMath::Abs(FloatArray[i] - Values[i]) >= Tolerance)
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