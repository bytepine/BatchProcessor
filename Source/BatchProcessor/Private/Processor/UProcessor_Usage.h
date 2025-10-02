// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorBase.h"
#include "UProcessor_Usage.generated.h"

/**
 * 蓝图统计器
 */
UCLASS(DisplayName="蓝图统计器")
class BATCHPROCESSOR_API UUProcessor_Usage : public UProcessorBase
{
	GENERATED_BODY()

protected:
	/**
	 * 批处理开始
	 */
	virtual void OnStart() const override;

	/**
	 * 处理逻辑
	 * @param Pointer 处理对象
	 * @param Struct 处理结构
	 * @return 是否有修改
	 */
	virtual bool OnProcessing(void* Pointer, const UStruct* Struct) const override;
	
	/**
	 * 批处理结束
	 */
	virtual void OnFinish() const override;
	
	TSet<FString> BlueprintSet;
};
