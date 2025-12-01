// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchFunctionLibrary.h"

#include "BatchDefine.h"
#include "ConditionBase.h"
#include "ProcessorBase.h"

bool UBatchFunctionLibrary::DoProcessor(const UProcessorBase* Processor, const UBlueprint* Assets, UBatchContext* Context,
	const FBatchVariable& Variable)
{
	return Processor->Processing(Assets, Context, Variable);
}

template <typename SPBatchProcessorType>
bool UBatchFunctionLibrary::DoProcessors(const TArray<SPBatchProcessorType*>& Processors, const UBlueprint* Assets, UBatchContext* Context,
	const FBatchVariable& Variable)
{
	bool bResult = false;
	
	for (const UProcessorBase* Processor : Processors)
	{
		bResult |= Processor->Processing(Assets, Context, Variable);
	}
	
	return bResult;
}

bool UBatchFunctionLibrary::FindProperty(const FString& PropertyName, const FBatchVariable& Variable, FBatchProperty& FindProperty)
{
	if (!Variable.Address|| !Variable.Struct)
	{
		return false;
	}

	// 分割属性路径
	TArray<FString> PropertyPath;
	PropertyName.ParseIntoArray(PropertyPath, TEXT("."));

	if (PropertyPath.IsEmpty())
	{
		return false;
	}

	// 初始化查找容器和结构
	void* CurrentContainer = Variable.Address;
	const UStruct* CurrentStruct = Variable.Struct;

	// 遍历属性路径
	for (int32 i = 0; i < PropertyPath.Num(); ++i)
	{
		FString CurrentName = PropertyPath[i];
		int32 ArrayIndex = INDEX_NONE;
		
		// 检查是否是数组元素访问 (如 "ArrayName[0]")
		const int32 OpenBracketPos = CurrentName.Find(TEXT("["));
		if (OpenBracketPos != INDEX_NONE && CurrentName.EndsWith(TEXT("]")))
		{
			// 提取数组索引
			const FString IndexStr = CurrentName.Mid(OpenBracketPos + 1, CurrentName.Len() - OpenBracketPos - 2);
			ArrayIndex = FCString::Atoi(*IndexStr);
			
			// 更新当前属性名为数组名 (去掉索引部分)
			CurrentName = CurrentName.Left(OpenBracketPos);
		}

		// 查找属性(包括继承的属性)
		FProperty* CurrentProperty = FindPropertyByName(CurrentStruct, CurrentName);
		if (!CurrentProperty)
		{
			return false;
		}

		// 处理数组索引
		if (ArrayIndex != INDEX_NONE)
		{
			if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(CurrentProperty))
			{
				FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(CurrentContainer));
				if (!ArrayHelper.IsValidIndex(ArrayIndex))
				{
					return false;
				}
				CurrentContainer = ArrayHelper.GetRawPtr(ArrayIndex);
				CurrentProperty = ArrayProperty->Inner;
			}
			else
			{
				// 不是数组属性但有数组索引语法
				return false;
			}
		}

		// 如果是最后一个属性，直接返回
		if (i == PropertyPath.Num() - 1)
		{
			FindProperty.Address = CurrentContainer;
			FindProperty.Property = CurrentProperty;
			return true;
		}

		// 如果不是最后一个属性，需要检查是否是对象/结构体属性
		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(CurrentProperty))
		{
			UObject* CurrentObject = ObjectProperty->GetObjectPropertyValue(CurrentProperty->ContainerPtrToValuePtr<void>(CurrentContainer));
			if (!IsValid(CurrentObject))
			{
				return false;
			}
			CurrentContainer = CurrentObject;
			CurrentStruct = CurrentObject->GetClass();
		}
		else if (const FStructProperty* StructProperty = CastField<FStructProperty>(CurrentProperty))
		{
			CurrentContainer = StructProperty->ContainerPtrToValuePtr<void>(CurrentContainer);
			CurrentStruct = StructProperty->Struct;
		}
		else
		{
			// 不是对象也不是结构体，无法继续深入查找
			return false;
		}
	}

	return false;
}

FProperty* UBatchFunctionLibrary::FindPropertyByName(const UStruct* Struct, const FString& PropertyName)
{
	for (TFieldIterator<FProperty> PropIt(Struct); PropIt; ++PropIt)
	{
		if (PropIt->GetName().StartsWith(PropertyName))
		{
			return *PropIt;
		}
	}
	return nullptr;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	const int64 Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}

	if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(TargetProperty.Property))
	{
		if (NumericProperty->IsInteger())
		{
			void* PropertyValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
			if (NumericProperty->GetSignedIntPropertyValue(PropertyValuePtr) == Value)
			{
				return EBatchSetPropertyResult::Same;
			}
			
			NumericProperty->SetIntPropertyValue(PropertyValuePtr, Value);
		}
	}
	
	return EBatchSetPropertyResult::Success;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	const bool Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}

	if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = BoolProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		if (BoolProperty->GetPropertyValue(PropertyValuePtr) == Value)
		{
			return EBatchSetPropertyResult::Same;
		}
		
		BoolProperty->SetPropertyValue(PropertyValuePtr, Value);
	}
	
	return EBatchSetPropertyResult::Success;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	const float Value)
{
	return SetProperty(PropertyName, Variable, static_cast<double>(Value));
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	const double Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}

	if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(TargetProperty.Property))
	{
		if (NumericProperty->IsFloatingPoint())
		{
			void* PropertyValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
			if (FMath::IsNearlyEqual(NumericProperty->GetFloatingPointPropertyValue(PropertyValuePtr), Value))
			{
				return EBatchSetPropertyResult::Same;
			}
			
			NumericProperty->SetFloatingPointPropertyValue(PropertyValuePtr, Value);
		}
	}
	
	return EBatchSetPropertyResult::Success;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
                                                           const FString& Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}

	if (const FStrProperty* StrProperty = CastField<FStrProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = StrProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		if (StrProperty->GetPropertyValue(PropertyValuePtr).Equals(Value))
		{
			return EBatchSetPropertyResult::Same;
		}
		
		StrProperty->SetPropertyValue(PropertyValuePtr, Value);
	}
	else if (const FNameProperty* NameProperty = CastField<FNameProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = NameProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		if (NameProperty->GetPropertyValue(PropertyValuePtr).IsEqual(FName(*Value)))
		{
			return EBatchSetPropertyResult::Same;
		}
		
		NameProperty->SetPropertyValue(PropertyValuePtr, FName(*Value));
	}
	else if (const FTextProperty* TextProperty = CastField<FTextProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = TextProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		if (TextProperty->GetPropertyValue(PropertyValuePtr).ToString().Equals(Value))
		{
			return EBatchSetPropertyResult::Same;
		}
		
		TextProperty->SetPropertyValue(PropertyValuePtr, FText::FromString(Value));
	}
	else
	{
		return EBatchSetPropertyResult::Failed;
	}
	
	return EBatchSetPropertyResult::Success;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	UObject* Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}
	
	if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		if (ObjectProperty->GetPropertyValue(PropertyValuePtr) == Value)
		{
			return EBatchSetPropertyResult::Same;
		}
		
		ObjectProperty->SetPropertyValue(PropertyValuePtr, Value);
	}
	else
	{
		return EBatchSetPropertyResult::Failed;
	}
	
	return EBatchSetPropertyResult::Success;
}

EBatchSetPropertyResult UBatchFunctionLibrary::SetProperty(const FString& PropertyName, const FBatchVariable& Variable,
	const FSoftObjectPtr& Value)
{
	FBatchProperty TargetProperty;
	if (!FindProperty(PropertyName, Variable, TargetProperty))
	{
		return EBatchSetPropertyResult::NotFound;
	}
	
	if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(TargetProperty.Property))
	{
		void* PropertyValuePtr = SoftObjectProperty->ContainerPtrToValuePtr<void>(TargetProperty.Address);
		const FSoftObjectPtr& CurrentValue = SoftObjectProperty->GetPropertyValue(PropertyValuePtr);
		if (CurrentValue.GetLongPackageName() == Value.GetLongPackageName())
		{
			return EBatchSetPropertyResult::Same;
		}
		
		SoftObjectProperty->SetPropertyValue(PropertyValuePtr, Value);
	}
	else
	{
		return EBatchSetPropertyResult::Failed;
	}
	
	return EBatchSetPropertyResult::Success;
}

bool UBatchFunctionLibrary::CheckCondition(const TArray<UConditionBase*>& Conditions, const bool bMustPassAllCondition,
	const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable)
{
	bool bPass = true;
	
	if (!Conditions.IsEmpty())
	{
		if (bMustPassAllCondition)
		{
			for (UConditionBase* Condition : Conditions)
			{
				if (!Condition->CheckCondition(Assets, Context, Variable))
				{
					bPass = false;
					break;
				}
			}
		}
		else
		{
			bPass = false;
			for (UConditionBase* Condition : Conditions)
			{
				if (Condition->CheckCondition(Assets, Context, Variable))
				{
					bPass = true;
					break;
				}
			}
		}
	}

	return bPass;
}
