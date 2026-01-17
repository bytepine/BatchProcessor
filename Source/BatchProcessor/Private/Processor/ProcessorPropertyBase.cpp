// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ProcessorPropertyBase.h"

#include "BatchFunctionLibrary.h"

void UProcessorPropertyBase::LogResult(const EBatchSetPropertyResult Result) const
{
	switch (Result)
	{
	case EBatchSetPropertyResult::Success:
		UE_LOG(LogBatchProcessor, Warning, TEXT("SetBool: 设置属性成功 [%s]"), *PropertyName);
		break;
	case EBatchSetPropertyResult::Failed:
		UE_LOG(LogBatchProcessor, Warning, TEXT("SetBool: 设置属性失败 [%s]"), *PropertyName);
		break;
	case EBatchSetPropertyResult::Same:
		UE_LOG(LogBatchProcessor, Warning, TEXT("SetBool: 属性值相同 [%s]"), *PropertyName);
		break;
	case EBatchSetPropertyResult::NotFound:
		UE_LOG(LogBatchProcessor, Warning, TEXT("SetBool: 没找到属性 [%s]"), *PropertyName);
		break;
	}
}
