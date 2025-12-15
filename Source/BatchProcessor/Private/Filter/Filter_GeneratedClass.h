// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchDefine.h"
#include "FilterBase.h"
#include "Filter_GeneratedClass.generated.h"

/**
 * 生成类过滤器
 */
UCLASS(DisplayName="生成类过滤器")
class BATCHPROCESSOR_API UFilter_GeneratedClass : public UFilterBase
{
	GENERATED_BODY()
	
public:
	explicit UFilter_GeneratedClass(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual bool OnFilter(const UBlueprint* Blueprint) const override;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="比较")
	EClassComparisonOperators ComparisonOperator;
	
	UPROPERTY(EditDefaultsOnly, Category="参数", DisplayName="生成类")
	TSoftClassPtr<UObject> GeneratedClass;
};
