// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "Scanner_Directory.h"

#include "BatchDefine.h"
#include "Utils/BatchVersionCompat.h"
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

	// 资产文件类判断
	if (!IsValid(AssetFileClass))
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("OnScannerAssets: AssetFileClass Is InValid!"));
		return;
	}

	// 获取资产注册表
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// 构建过滤器（按资产文件类型）
	FARFilter Filter;
	Filter.PackagePaths.Add(*Directory.Path);
	Filter.bRecursivePaths   = bRecursivePaths;
	Filter.bRecursiveClasses = bRecursiveClasses;
#if BP_UE_HAS_CLASS_PATHS
	Filter.ClassPaths.Add(AssetFileClass->GetClassPathName());
#else
	Filter.ClassNames.Add(AssetFileClass->GetFName());
#endif

	TArray<FAssetData> AssetArray;
	AssetRegistry.GetAssets(Filter, AssetArray);

	// 蓝图父类二次过滤（通过 NativeParentClass Tag，无需加载资产）
	if (IsValid(BlueprintNativeParent))
	{
		const FString TargetParentPath = BlueprintNativeParent->GetPathName();

		AssetArray.RemoveAll([&](const FAssetData& Data) -> bool
		{
			// NativeParentClass tag 格式示例："/Script/Engine.Actor"
			FString NativeParent;
			Data.GetTagValue(TEXT("NativeParentClass"), NativeParent);
			if (NativeParent.IsEmpty()) return true; // 无 tag 则排除

			// 去掉 UClass 前缀（如 "Class'/Script/Engine.Actor'"）取路径部分
			NativeParent.RemoveFromStart(TEXT("Class'"));
			NativeParent.RemoveFromEnd(TEXT("'"));

			// 在内存中查找已知类（引擎核心类均已加载）
			const UClass* FoundClass = FindObject<UClass>(nullptr, *NativeParent);
			if (!FoundClass) return true;

			// 保留：FoundClass 是目标父类本身或其子类
			return !FoundClass->IsChildOf(BlueprintNativeParent);
		});
	}

	Assets.Append(AssetArray);
}
