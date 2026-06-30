// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ScannerBase.h"

#include "Internationalization/Regex.h"

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
	FilterAssetsByName(Assets, NameType, RegularExpressions);
}

void UScannerBase::FilterAssetsByName(TSet<FAssetData>& Assets, EFilter_NameType InNameType, const FString& Regex)
{
	// 正则表达式为空时不过滤
	if (Regex.IsEmpty())
	{
		return;
	}

	const FRegexPattern Pattern(Regex);

	TArray<FAssetData> AssetsToRemove;
	for (const FAssetData& Asset : Assets)
	{
		FString FilterName;
		switch (InNameType)
		{
		case EFilter_NameType::AssetName:
			FilterName = Asset.AssetName.ToString();
			break;
		case EFilter_NameType::PackageName:
			FilterName = Asset.PackageName.ToString();
			break;
		default:
			continue;
		}

		FRegexMatcher Matcher(Pattern, FilterName);
		if (!Matcher.FindNext())
		{
			AssetsToRemove.Add(Asset);
		}
	}

	for (const FAssetData& Asset : AssetsToRemove)
	{
		Assets.Remove(Asset);
	}
}
