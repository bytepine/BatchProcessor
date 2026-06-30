// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "BatchAssetTypeActions.h"
#include "BatchAsset.h"
#include "BatchAssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "BatchAssetTypeActions"

FText FBatchAssetTypeActions::GetName() const
{
    return LOCTEXT("BatchAssetTypeName", "批处理任务");
}

FColor FBatchAssetTypeActions::GetTypeColor() const
{
    return FColor(78, 152, 220);
}

UClass* FBatchAssetTypeActions::GetSupportedClass() const
{
    return UBatchAsset::StaticClass();
}

uint32 FBatchAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::Misc;
}

void FBatchAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                              TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
    const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
        ? EToolkitMode::WorldCentric
        : EToolkitMode::Standalone;

    for (UObject* Obj : InObjects)
    {
        if (UBatchAsset* Asset = Cast<UBatchAsset>(Obj))
        {
            TSharedRef<FBatchAssetEditorToolkit> Editor = MakeShared<FBatchAssetEditorToolkit>();
            Editor->InitBatchAssetEditor(Mode, EditWithinLevelEditor, Asset);
        }
    }
}

void FBatchAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
    TArray<TWeakObjectPtr<UBatchAsset>> Assets;
    for (UObject* Obj : InObjects)
    {
        if (UBatchAsset* A = Cast<UBatchAsset>(Obj))
        {
            Assets.Add(A);
        }
    }
    if (Assets.Num() == 0) return;

    Section.AddMenuEntry(
        "BatchAsset_Run",
        LOCTEXT("RunBatch", "执行批处理"),
        LOCTEXT("RunBatch_Tooltip", "立即运行此批处理任务"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateLambda([Assets]()
        {
            for (const TWeakObjectPtr<UBatchAsset>& AssetPtr : Assets)
            {
                if (UBatchAsset* A = AssetPtr.Get())
                {
                    A->Start();
                }
            }
        }))
    );

    Section.AddMenuEntry(
        "BatchAsset_DryRun",
        LOCTEXT("DryRunBatch", "试运行（不保存）"),
        LOCTEXT("DryRunBatch_Tooltip", "以试运行模式执行，不落盘"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateLambda([Assets]()
        {
            for (const TWeakObjectPtr<UBatchAsset>& AssetPtr : Assets)
            {
                if (UBatchAsset* A = AssetPtr.Get())
                {
                    A->StartWithReporter(nullptr, true);
                }
            }
        }))
    );
}

#undef LOCTEXT_NAMESPACE
