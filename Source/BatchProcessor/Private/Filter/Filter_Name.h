// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FilterBase.h"
#include "Filter_Name.generated.h"

UENUM(DisplayName="过滤名称类型")
enum class EFilter_NameType
{
	AssetName,
	PackageName,
};

/**
 * 名称过滤器
 */
UCLASS(DisplayName="名称过滤器")
class BATCHPROCESSOR_API UFilter_Name : public UFilterBase
{
	GENERATED_BODY()

public:
	explicit UFilter_Name(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void OnFilter(TSet<FAssetData>& Assets) const override;

	UPROPERTY(EditDefaultsOnly, Category="过滤参数", DisplayName="过滤名称类型")
	EFilter_NameType NameType;
	
	UPROPERTY(EditDefaultsOnly, Category="过滤参数", DisplayName="正则表达式")
	FString RegularExpressions;
};
