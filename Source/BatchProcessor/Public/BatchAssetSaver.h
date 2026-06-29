// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 资产保存结果
 */
enum class EBatchSaveResult : uint8
{
	/** 成功落盘 */
	Success,
	/** 保存失败 */
	Failed,
	/** 跳过（如试运行，或包无需保存） */
	Skipped
};

/**
 * 批处理资产保存接口
 *
 * 把"如何持久化被修改的资产"从调度核心中解耦：
 * 默认实现关闭资产编辑器并落盘；试运行实现只记录不落盘；
 * 也可扩展为接入源码管理 checkout 等策略。
 */
class BATCHPROCESSOR_API IBatchAssetSaver
{
public:
	virtual ~IBatchAssetSaver() = default;

	/**
	 * 保存被修改的资产
	 * @param Asset 已被处理器修改的资产对象
	 * @return 保存结果
	 */
	virtual EBatchSaveResult SaveAsset(UObject* Asset) = 0;
};

/**
 * 默认保存实现：标脏 → 关闭该资产的所有编辑器 → SavePackage 落盘
 */
class BATCHPROCESSOR_API FDefaultBatchAssetSaver : public IBatchAssetSaver
{
public:
	virtual EBatchSaveResult SaveAsset(UObject* Asset) override;
};

/**
 * 试运行实现：只记录将要保存的资产，不做任何落盘与标脏
 */
class BATCHPROCESSOR_API FDryRunBatchAssetSaver : public IBatchAssetSaver
{
public:
	virtual EBatchSaveResult SaveAsset(UObject* Asset) override;
};
