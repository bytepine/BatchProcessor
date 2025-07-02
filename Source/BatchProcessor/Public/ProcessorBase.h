// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProcessorBase.generated.h"

class UBatchBase;

/**
 * 处理器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UProcessorBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 批处理开始
	 */
	void Start() const;
	
	/**
	 * 处理逻辑
	 * @param Pointer 处理对象
	 * @param Struct 处理结构
	 * @return 是否有修改
	 */
	bool Processing(void* Pointer, const UStruct* Struct) const;
	
	/**
	 * 批处理结束
	 */
	void Finish() const;
protected:
	/**
	 * 批处理开始
	 */
	virtual void OnStart() const;

	/**
	 * 处理逻辑
	 * @param Pointer 处理对象
	 * @param Struct 处理结构
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(void* Pointer, const UStruct* Struct) const;
	
	/**
	 * 批处理结束
	 */
	virtual void OnFinish() const;
};

/**
 * 通用处理器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UCommonProcessorBase : public UProcessorBase
{
	GENERATED_BODY()
};