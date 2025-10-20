// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProcessorBase.generated.h"

struct FBatchVariable;
class UBatchContext;
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
	 * @param Context 上下文
	 * 批处理开始
	 */
	void Start(UBatchContext* Context) const;
	
	/**
	 * 处理逻辑
	 * @param Assets 蓝图资产
	 * @param Context 上下文
	 * @param Variable 变量
	 * @return 是否有修改
	 */
	bool Processing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const;
	
	/**
	 * @param Context 上下文
	 * 批处理完成
	 */
	void Finish(UBatchContext* Context) const;
protected:
	/**
	 * @param Context 上下文
	 * 批处理开始
	 */
	virtual void OnStart(UBatchContext* Context) const;

	/**
	 * 处理逻辑
	 * @param Assets 蓝图资产
	 * @param Context 上下文
	 * @param Variable 变量
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(const UBlueprint* Assets, UBatchContext* Context, const FBatchVariable& Variable) const;
	
	/**
	 * @param Context 上下文
	 * 批处理完成
	 */
	virtual void OnFinish(UBatchContext* Context) const;
	
	/**
	 * 获取子处理器
	 * @param SubProcessors 子处理器
	 */
	virtual void GetSubProcessors(TArray<UProcessorBase*>& SubProcessors) const;
};