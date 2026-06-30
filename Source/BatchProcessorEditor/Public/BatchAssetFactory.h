// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BatchAssetFactory.generated.h"

/**
 * Content Browser 右键 → 新建 → 批处理任务
 */
UCLASS()
class BATCHPROCESSOREDITOR_API UBatchAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UBatchAssetFactory();

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                      EObjectFlags Flags, UObject* Context,
                                      FFeedbackContext* Warn) override;
    virtual bool ShouldShowInNewMenu() const override { return true; }
};
