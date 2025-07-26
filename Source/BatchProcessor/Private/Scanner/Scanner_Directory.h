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
	virtual bool OnScannerAssets(TSet<FAssetData>& Assets) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(ContentDir, DisplayName="目录"))
	FDirectoryPath Directory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="蓝图类"))
	TSubclassOf<UBlueprint> BlueprintClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="递归搜索子文件夹"))
	uint8 bRecursivePaths : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="扫描", meta=(DisplayName="是否包含子类"))
	uint8 bRecursiveClasses : 1;
};
