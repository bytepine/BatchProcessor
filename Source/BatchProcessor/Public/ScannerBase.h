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
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UScannerBase : public UObject
{
	GENERATED_BODY()

public:
	explicit UScannerBase(const FObjectInitializer& ObjectInitializer);
	
	void ScannerAssets(TSet<FAssetData>& Assets) const;

protected:
	virtual void OnScannerAssets(TSet<FAssetData>& Assets) const;
	
	virtual void OnFilter(TSet<FAssetData>& Assets) const;
	
	UPROPERTY(EditDefaultsOnly, Category="过滤", DisplayName="过滤名称类型")
	EFilter_NameType NameType;
	
	UPROPERTY(EditDefaultsOnly, Category="过滤", DisplayName="正则表达式")
	FString RegularExpressions;
};
