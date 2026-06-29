// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchAssetSaver.h"

#include "BatchDefine.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UObject/SavePackage.h"

EBatchSaveResult FDefaultBatchAssetSaver::SaveAsset(UObject* Asset)
{
	if (!IsValid(Asset))
	{
		return EBatchSaveResult::Failed;
	}

	// 标记包为脏；若无需标脏则跳过保存（保持原行为）
	if (!Asset->MarkPackageDirty())
	{
		return EBatchSaveResult::Skipped;
	}

	UPackage* Package = Asset->GetOutermost();
	if (!Package)
	{
		return EBatchSaveResult::Failed;
	}

	const FString Filename = FPackageName::LongPackageNameToFilename(
		Package->GetName(),
		FPackageName::GetAssetPackageExtension());

	if (GEditor && GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(Asset);
	}

	if (!UPackage::SavePackage(Package, nullptr, *Filename, FSavePackageArgs()))
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
