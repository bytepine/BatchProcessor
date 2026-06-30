// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchBase.h"
#include "BatchDefine.h"
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

	/** 已处理（调度）的资产数量，供编辑器进度条计算百分比 */
	int32 GetCount() const { return Count; }

	bool IsLoadFinish() const;

	FString GetProgress() const;

	/** 获取批处理结果（可读写） */
	FBatchResult& GetResult() { return Result; }

	/** 获取批处理结果（只读） */
	const FBatchResult& GetResult() const { return Result; }

	/**
	 * 获取（或懒创建）处理器私有的便笺簿
	 *
	 * 模板定义在头文件，所有翻译单元均可实例化，支持任意 UBatchScratchPad 子类。
	 * 典型用法：在 OnProcessing/OnFinish 中通过 `Context->GetScratchPad<UMyPad>(this)` 取跨资产共享状态。
	 */
	template<typename BatchScratchPadType = UBatchScratchPad>
	BatchScratchPadType* GetScratchPad(const IBatchScratchPadInterface* Owner)
	{
		if (!Owner)
		{
			return nullptr;
		}

		UBatchScratchPad* ScratchPad = nullptr;
		const int64 UID = Owner->GetUID();
		if (!ScratchPadMap.Contains(UID))
		{
			ScratchPad = NewObject<UBatchScratchPad>(this, Owner->GetScratchPadClass());
			ScratchPadMap.Add(UID, ScratchPad);
		}
		else
		{
			ScratchPad = ScratchPadMap[UID];
		}
		return Cast<BatchScratchPadType>(ScratchPad);
	}
	
protected:
	friend UBatchBase;
	friend class UBatchRunner;
	
	void Initialized(const TSet<FAssetData>& InAssetSet);
	
	int32 GetPendingArray(TArray<FSoftObjectPath>& PendingArray, int32 MaxCount);

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

	/**
	 * 批处理结果
	 */
	FBatchResult Result;
};
