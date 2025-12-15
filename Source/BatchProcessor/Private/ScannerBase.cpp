// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ScannerBase.h"

UScannerBase::UScannerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NameType(EFilter_NameType::AssetName)
{
	
}

void UScannerBase::ScannerAssets(TSet<FAssetData>& Assets) const
{
	OnScannerAssets(Assets);
	OnFilter(Assets);
}

void UScannerBase::OnScannerAssets(TSet<FAssetData>& Assets) const
{
	
}

void UScannerBase::OnFilter(TSet<FAssetData>& Assets) const
{
	// 如果正则表达式为空，则不进行过滤
	if (RegularExpressions.IsEmpty())
	{
		return;
	}

	// 编译正则表达式
	const FRegexPattern Pattern(RegularExpressions);

	// 遍历资产集合并进行过滤
	TArray<FAssetData> AssetsToRemove;
	for (const FAssetData& Asset : Assets)
	{
		// 获取当前过滤名
		FString FilterName;
		switch (NameType) {
		case EFilter_NameType::AssetName:
			FilterName = Asset.AssetName.ToString();
			break;
		case EFilter_NameType::PackageName:
			FilterName = Asset.PackageName.ToString();
			break;
		default:
			continue;
		}

		// 如果不匹配正则表达式，则标记为需要移除
		if (FRegexMatcher Matcher(Pattern, FilterName); !Matcher.FindNext())
		{
			AssetsToRemove.Add(Asset);
		}
	}
	
	// 移除不匹配的资产
	for (const FAssetData& Asset : AssetsToRemove)
	{
		Assets.Remove(Asset);
	}
}
