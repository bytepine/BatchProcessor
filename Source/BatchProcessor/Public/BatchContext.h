// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchBase.h"
#include "UObject/Object.h"
#include "BatchContext.generated.h"

class IBatchScratchPadInterface;
class UBatchScratchPad;
/**
 * 批处理上下文
 */
UCLASS()
class BATCHPROCESSOR_API UBatchContext : public UObject
{
	GENERATED_UCLASS_BODY()

	const TSet<FAssetData>& GetAssetSet() const { return AssetSet; }

	const TArray<FAssetData>& GetLoadArray() const { return LoadArray; }
	
	int32 GetTotal() const { return Total; }

	bool IsLoadFinish() const;

	FString GetProgress() const;

	template<typename BatchScratchPadType = UBatchScratchPad>
	BatchScratchPadType* GetScratchPad(const IBatchScratchPadInterface* Owner);
	
protected:
	friend UBatchBase;
	
	void Initialized(const TSet<FAssetData>& InAssetSet);
	
	int32 GetPendingArray(TArray<FSoftObjectPath>& PendingArray);

	int32 AddCount();
private:
	/**
	 * 处理计数
	 */
	int32 Count;
	
	/**
	 * 最大计数
	 */
	int32 Total;
	
	/**
	 * 资产列表
	 */
	TSet<FAssetData> AssetSet;

	/**
	 * 待加载列表
	 */
	TArray<FAssetData> LoadArray;

	UPROPERTY()
	TMap<int64, UBatchScratchPad*> ScratchPadMap;
};
