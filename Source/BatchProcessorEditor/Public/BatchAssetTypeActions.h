// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

/**
 * 注册 UBatchAsset 到 Content Browser：
 * - 分类颜色
 * - 双击打开 FBatchAssetEditorToolkit
 * - 右键菜单（执行、试运行）
 */
class BATCHPROCESSOREDITOR_API FBatchAssetTypeActions : public FAssetTypeActions_Base
{
public:
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
                                 TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
    virtual uint32 GetCategories() override;
    virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
    virtual void GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section) override;
};
