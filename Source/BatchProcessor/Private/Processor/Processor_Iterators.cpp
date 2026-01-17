// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Processor_Iterators.h"

#include "BatchFunctionLibrary.h"

bool UProcessor_Iterators::OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const
{
	bool bResult = Super::OnProcessing(Assets, Context, Variable);
	
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
			bResult |= DoProcessor(Assets, Context, FBatchProperty(ElementPtr, ArrayProperty->Inner));
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
				bResult |= DoProcessor(Assets, Context, FBatchProperty(ElementPtr, SetProperty->ElementProp));
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
				bResult |= DoProcessor(Assets, Context, FBatchProperty(ValuePtr, MapProperty->ValueProp));
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

bool UProcessor_Iterators::DoProcessor(const UBlueprint* Assets, UBatchContext* Context,
	const FBatchProperty& Property) const
{
	if (!Property.IsValid()) return false;
	
	// 获取属性本身的类型结构
	FBatchVariable Variable(Property);
	if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property.Property))
	{
		Variable = FBatchVariable(StructProperty->ContainerPtrToValuePtr<void>(Property.Address), StructProperty->Struct);
	}
	else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property.Property))
	{
		UObject* CurrentObject = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(Property.Address));
		if (!IsValid(CurrentObject))
		{
			return false;
		}
		Variable = FBatchVariable(CurrentObject, CurrentObject->GetClass());
	}

	if (!Variable.IsValid()) return false;
	
	return UBatchFunctionLibrary::DoProcessors(Processors, Assets, Context, Variable);
}

