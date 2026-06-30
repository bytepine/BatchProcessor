// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/BatchTestCompat.h"
#include "Tests/BatchTestFixtures.h"
#include "BatchDefine.h"
#include "FilterBase.h"
#include "UObject/UObjectIterator.h"

// ─────────────────────────────────────────────────────────────────────────────
// Filter_GeneratedClass — 未配置类时应排除所有资产
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchFilter_GeneratedClass_ExcludesWhenNotConfigured,
	"BatchProcessor.Filter.GeneratedClass.ExcludesWhenNotConfigured",
	BATCH_AUTOMATION_TEST_FLAGS)

bool FBatchFilter_GeneratedClass_ExcludesWhenNotConfigured::RunTest(const FString& Parameters)
{
	UClass* FilterClass = FindObject<UClass>(nullptr, TEXT("/Script/BatchProcessor.Filter_GeneratedClass"));
	if (!FilterClass)
	{
		for (TObjectIterator<UClass> It; It; ++It)
		{
			if (It->GetName() == TEXT("Filter_GeneratedClass"))
			{
				FilterClass = *It;
				break;
			}
		}
	}
	TestNotNull(TEXT("Filter_GeneratedClass 类应已注册"), FilterClass);

	FStaticConstructObjectParameters FilterParams(FilterClass);
	UFilterBase* Filter = Cast<UFilterBase>(StaticConstructObject_Internal(FilterParams));
	TestNotNull(TEXT("应能实例化 Filter_GeneratedClass"), Filter);
	Filter->AddToRoot();

	UBatchTestObject* DummyObj = NewObject<UBatchTestObject>();
	const FBatchTarget Target(DummyObj);

	const bool bShouldKeep = Filter->ShouldKeep(Target);
	TestFalse(TEXT("未配置 GeneratedClass 时 ShouldKeep 应返回 false（排除）"), bShouldKeep);

	Filter->RemoveFromRoot();
	return true;
}
