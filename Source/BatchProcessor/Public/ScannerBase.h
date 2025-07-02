// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ScannerBase.generated.h"

/**
 * 扫描器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UScannerBase : public UObject
{
	GENERATED_BODY()

public:
	bool ScannerAssets(TSet<FAssetData>& Assets);

protected:
	virtual bool OnScannerAssets(TSet<FAssetData>& Assets);
};
