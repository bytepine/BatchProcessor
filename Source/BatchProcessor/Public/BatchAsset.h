// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchBase.h"
#include "BatchAsset.generated.h"

/**
 * 批处理资产
 *
 * 以独立 UAsset 形式存储批处理配置（扫描器 / 过滤器 / 处理器）。
 * 区别于蓝图子类方式，本类直接在 Content Browser 中创建实例，
 * 并通过专属的 FBatchAssetEditorToolkit 打开可视化编辑器。
 *
 * 注意：不可通过蓝图继承（非 Blueprintable），扩展逻辑请使用组件层。
 */
UCLASS(BlueprintType, meta=(DisplayName="批处理任务"))
class BATCHPROCESSOR_API UBatchAsset : public UBatchBase
{
	GENERATED_BODY()

public:
	/** 任务描述（仅供说明，不影响运行） */
	UPROPERTY(EditAnywhere, Category="概述", meta=(DisplayName="描述", MultiLine=true))
	FString Description;
};
