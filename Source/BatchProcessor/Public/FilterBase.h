// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FilterBase.generated.h"

/**
 * 筛选器基类
 */
UCLASS(Abstract, EditInlineNew)
class BATCHPROCESSOR_API UFilterBase : public UObject
{
	GENERATED_BODY()

public:
	bool Filter(const UBlueprint* Blueprint) const;

protected:
	virtual bool OnFilter(const UBlueprint* Blueprint) const;
};
