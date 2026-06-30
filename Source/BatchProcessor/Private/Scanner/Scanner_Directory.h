// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScannerBase.h"
#include "Scanner_Directory.generated.h"

/**
 * 目录搜索器
 */
UCLASS(DisplayName="目录搜索器")
class BATCHPROCESSOR_API UScanner_Directory : public UScannerBase
{
	GENERATED_BODY()

public:
	explicit UScanner_Directory(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnScannerAssets(TSet<FAssetData>& Assets) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(ContentDir, DisplayName="目录"))
	FDirectoryPath Directory;

	/**
	 * 资产文件类型——Content Browser 中实际存储的 UObject 子类。
	 * 例：Blueprint（蓝图）、StaticMesh（静态网格）、Material（材质）、DataTable……
	 * 注意：这是「文件格式」，不是「游戏类父类」；如需按 Actor 父类筛选蓝图，
	 * 请设此字段为 Blueprint，并在「蓝图父类」填写 Actor。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="资产文件类"))
	TSubclassOf<UObject> AssetFileClass;

	/**
	 * 蓝图父类过滤（仅当资产文件类为 Blueprint 时生效）。
	 * 通过 AssetRegistry 的 NativeParentClass Tag 在不加载资产的情况下筛选，
	 * 例：填 Actor 可找到所有继承自 Actor 的蓝图。
	 * 留空则不按父类过滤。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="蓝图父类（不加载筛选）"))
	TSubclassOf<UObject> BlueprintNativeParent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="递归搜索子文件夹"))
	uint8 bRecursivePaths : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="包含子类资产"))
	uint8 bRecursiveClasses : 1;
};
