// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Scanner_Directory.h"

#include "BatchProcessor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

UScanner_Directory::UScanner_Directory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bRecursivePaths(true)
	, bRecursiveClasses(true)
{
	
}

void UScanner_Directory::OnScannerAssets(TSet<FAssetData>& Assets) const
{
	// 路径判断
	if (Directory.Path.IsEmpty())
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnScannerAssets: Directory Is Empty!"));
		return;
	}

	// 类判断
	if (!IsValid(BlueprintClass))
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnScannerAssets: BlueprintClass Is InValid!"));
		return;
	}

	// 获取资产注册表
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// 构建过滤器
	FARFilter Filter;
	Filter.PackagePaths.Add(*Directory.Path);
	Filter.bRecursivePaths = bRecursivePaths;  // 是否递归搜索子文件夹
	Filter.ClassPaths.Add(BlueprintClass->GetClassPathName());
	Filter.bRecursiveClasses = bRecursiveClasses;  // 是否包含子类

	TArray<FAssetData> AssetArray;
	AssetRegistry.GetAssets(Filter, AssetArray);
	
	Assets.Append(AssetArray);
}
