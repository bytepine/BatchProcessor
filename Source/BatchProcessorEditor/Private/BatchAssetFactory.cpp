// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "BatchAssetFactory.h"
#include "BatchAsset.h"

UBatchAssetFactory::UBatchAssetFactory()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UBatchAsset::StaticClass();
}

UObject* UBatchAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                               EObjectFlags Flags, UObject* Context,
                                               FFeedbackContext* Warn)
{
    return NewObject<UBatchAsset>(InParent, Class, Name, Flags | RF_Transactional);
}
