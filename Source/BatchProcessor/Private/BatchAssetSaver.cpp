// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchAssetSaver.h"

#include "BatchDefine.h"
#include "Utils/BatchVersionCompat.h"
#include "UObject/SavePackage.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"

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

	// 关闭该资产已打开的编辑器窗口，避免「编辑器持有数据→保存冲突」或保存后编辑器状态不同步
	if (GEditor)
	{
		if (UAssetEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			EditorSubsystem->CloseAllEditorsForAsset(Asset);
		}
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
