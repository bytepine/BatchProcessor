// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ScannerBase.generated.h"

UENUM(DisplayName="过滤名称类型")
enum class EFilter_NameType
{
	AssetName,
	PackageName,
};

/**
 * 扫描器基类
 *
 * 职责：**pre-load 过滤**——基于 FAssetData（尚未加载到内存）收集与初步筛选资产。
 * 对应地，UFilterBase 做 post-load 过滤（基于已加载的 FBatchTarget）。
 * 两层分工：Scanner 尽量在加载前剪枝以减少内存开销，Filter 做精细的运行时条件判断。
 *
 * 子类重写 OnScannerAssets 收集资产；基类 OnFilter 用 RegularExpressions 对名称做正则初筛。
 * OnFilter 的逻辑可通过 FilterAssetsByName 静态方法复用（自定义 Scanner 可直接调用）。
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UScannerBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UScannerBase(const FObjectInitializer& ObjectInitializer);
	
	void ScannerAssets(TSet<FAssetData>& Assets) const;

	/**
	 * 按名称正则过滤资产集合（pre-load 阶段通用工具，可供子类 Scanner 直接调用）
	 * @param Assets      待过滤的资产集合（原地修改，移除不匹配的条目）
	 * @param NameType    使用 AssetName 还是 PackageName 做匹配
	 * @param Regex       正则表达式字符串；为空时不过滤
	 */
	static void FilterAssetsByName(TSet<FAssetData>& Assets, EFilter_NameType NameType, const FString& Regex);

	/**
	 * 编辑期配置校验（可选实现）
	 * 将发现的问题追加到 OutErrors，无问题则不写入。
	 * 由编辑器在流水线视图刷新时聚合调用，不在运行路径上执行。
	 */
	virtual void ValidateConfig(TArray<FText>& OutErrors) const {}

protected:
	virtual void OnScannerAssets(TSet<FAssetData>& Assets) const;
	
	/** 基于 RegularExpressions 对 Assets 做正则名称初筛（调用 FilterAssetsByName） */
	virtual void OnFilter(TSet<FAssetData>& Assets) const;
	
	UPROPERTY(EditDefaultsOnly, Category="过滤", DisplayName="过滤名称类型")
	EFilter_NameType NameType;
	
	UPROPERTY(EditDefaultsOnly, Category="过滤", DisplayName="正则表达式")
	FString RegularExpressions;
};
