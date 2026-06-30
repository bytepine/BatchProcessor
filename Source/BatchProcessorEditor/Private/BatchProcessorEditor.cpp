// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "BatchProcessorEditor.h"
#include "BatchAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FBatchProcessorEditorModule"

void FBatchProcessorEditorModule::StartupModule()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    TSharedRef<FBatchAssetTypeActions> Actions = MakeShared<FBatchAssetTypeActions>();
    AssetTypeActions = Actions;
    AssetTools.RegisterAssetTypeActions(Actions);
}

void FBatchProcessorEditorModule::ShutdownModule()
{
    if (FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
    {
        if (AssetTypeActions.IsValid())
        {
            AssetToolsModule->Get().UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
        }
    }
    AssetTypeActions.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBatchProcessorEditorModule, BatchProcessorEditor)
