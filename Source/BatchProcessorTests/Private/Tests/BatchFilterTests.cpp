// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BatchDefine.h"
#include "Filter/Filter_GeneratedClass.h"

// ─────────────────────────────────────────────────────────────────────────────
// Filter_GeneratedClass — 未配置类时应排除所有资产
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchFilter_GeneratedClass_ExcludesWhenNotConfigured,
	"BatchProcessor.Filter.GeneratedClass.ExcludesWhenNotConfigured",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchFilter_GeneratedClass_ExcludesWhenNotConfigured::RunTest(const FString& Parameters)
{
	// GeneratedClass 未设置的过滤器应排除所有资产（返回 false）
	// 避免「忘配类 = 静默放行全部」的危险默认行为
	UFilter_GeneratedClass* Filter = NewObject<UFilter_GeneratedClass>();
	Filter->AddToRoot();

	// 用一个合法但无关的 UObject 构造 Target
	UObject* DummyObj = NewObject<UObject>(GetTransientPackage(), NAME_None, RF_Transient);
	const FBatchTarget Target(DummyObj);

	const bool bShouldKeep = Filter->ShouldKeep(Target);
	TestFalse(TEXT("未配置 GeneratedClass 时 ShouldKeep 应返回 false（排除）"), bShouldKeep);

	Filter->RemoveFromRoot();
	return true;
}
