// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchAssetSaver.h"

#include "BatchDefine.h"
#include "Utils/BatchVersionCompat.h"
#include "UObject/SavePackage.h"

EBatchSaveResult FDefaultBatchAssetSaver::SaveAsset(UObject* Asset)
{
	if (!IsValid(Asset))
	{
		return EBatchSaveResult::Failed;
	}

	UPackage* Package = Asset->GetOutermost();
	if (!Package)
	{
		return EBatchSaveResult::Failed;
	}

	Asset->MarkPackageDirty();

	const FString Filename = FPackageName::LongPackageNameToFilename(
		Package->GetName(),
		FPackageName::GetAssetPackageExtension());

#if BP_UE_HAS_SAVE_PACKAGE_ARGS
	if (!UPackage::SavePackage(Package, nullptr, *Filename, FSavePackageArgs()))
#else
	if (!UPackage::SavePackage(Package, Asset, RF_Standalone, *Filename))
#endif
	{
		UE_LOG(LogBatchProcessor, Error, TEXT("SaveAsset: Save File Failed [%s]"), *Filename);
		return EBatchSaveResult::Failed;
	}

	UE_LOG(LogBatchProcessor, Log, TEXT("SaveAsset: Save File [%s]"), *Filename);
	return EBatchSaveResult::Success;
}

EBatchSaveResult FDryRunBatchAssetSaver::SaveAsset(UObject* Asset)
{
	if (!IsValid(Asset))
	{
		return EBatchSaveResult::Failed;
	}

	UE_LOG(LogBatchProcessor, Log, TEXT("SaveAsset[DryRun]: Would Save [%s]"), *Asset->GetName());
	return EBatchSaveResult::Skipped;
}
