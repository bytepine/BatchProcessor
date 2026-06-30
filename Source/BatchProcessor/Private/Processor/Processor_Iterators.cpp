// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Iterators.h"

#include "BatchFunctionLibrary.h"

bool UProcessor_Iterators::OnProcessing(const FBatchTarget& Target, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Target, Context, Variable);
	
	FBatchProperty Property;
	UBatchFunctionLibrary::FindProperty(PropertyName, Variable, Property);
	if (!Property.IsValid())
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("Iterators: 没找到属性 [%s]"), *PropertyName);
		return bResult;
	}
	
	// 处理容器属性
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property.Property))
	{
		UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: 遍历Array"));
		
		FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Property.Address));
		for (int32 i = 0; i < ArrayHelper.Num(); ++i)
		{
			UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: [%s][%d]"), *PropertyName, i);
			
			void* ElementPtr = ArrayHelper.GetRawPtr(i);
			bResult |= DoProcessor(Target, Context, FBatchProperty(ElementPtr, ArrayProperty->Inner));
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property.Property))
	{
		UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: 遍历Set"));
		
		FScriptSetHelper SetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(Property.Address));
		for (int32 i = 0; i < SetHelper.Num(); ++i)
		{
			if (SetHelper.IsValidIndex(i))
			{
				UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: [%s][%d]"), *PropertyName, i);
				
				void* ElementPtr = SetHelper.GetElementPtr(i);
				bResult |= DoProcessor(Target, Context, FBatchProperty(ElementPtr, SetProperty->ElementProp));
			}
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property.Property))
	{
		UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: 遍历Map"));
		
		FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(Property.Address));
		for (int32 i = 0; i < MapHelper.Num(); ++i)
		{
			if (MapHelper.IsValidIndex(i))
			{
				UE_LOG(LogBatchProcessor, Log, TEXT("Iterators: [%s][%d]"), *PropertyName, i);
				
				void* ValuePtr = MapHelper.GetValuePtr(i);
				bResult |= DoProcessor(Target, Context, FBatchProperty(ValuePtr, MapProperty->ValueProp));
			}
		}
	}
	else
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("Iterators: 该属性不是容器类型 %s"), *PropertyName);
	}

	return bResult;
}

void UProcessor_Iterators::GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const
{
	Super::GetSubProcessors(SubProcessors);
	
	SubProcessors.Append(Processors);
}

bool UProcessor_Iterators::DoProcessor(const FBatchTarget& Target, UBatchContext* Context,
	const FBatchProperty& Property) const
{
	if (!Property.IsValid()) return false;
	
	// 仅支持 struct / object 类型元素；基元类型（int、float、FString 等）
	// 无法安全构造 FBatchVariable，明确报错跳过以防止非法内存寻址。
	if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property.Property))
	{
		const FBatchVariable Variable(
			StructProperty->ContainerPtrToValuePtr<void>(Property.Address),
			StructProperty->Struct);
		if (!Variable.IsValid()) return false;
		return UBatchFunctionLibrary::DoProcessors(Processors, Target, Context, Variable);
	}

	if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property.Property))
	{
		UObject* CurrentObject = ObjectProperty->GetObjectPropertyValue(
			ObjectProperty->ContainerPtrToValuePtr<void>(Property.Address));
		if (!IsValid(CurrentObject)) return false;
		const FBatchVariable Variable(CurrentObject, CurrentObject->GetClass());
		if (!Variable.IsValid()) return false;
		return UBatchFunctionLibrary::DoProcessors(Processors, Target, Context, Variable);
	}

	UE_LOG(LogBatchProcessor, Error,
		TEXT("Iterators: Element type of [%s] is not struct or object — primitive container iteration is not supported"),
		*Property.Property->GetName());
	return false;
}

