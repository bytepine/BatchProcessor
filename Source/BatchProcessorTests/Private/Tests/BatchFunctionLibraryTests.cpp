// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/BatchTestFixtures.h"
#include "BatchFunctionLibrary.h"
#include "BatchDefine.h"

// ─────────────────────────────────────────────────────────────────────────────
// FindProperty — 精确匹配
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchFindProperty_ExactMatch,
	"BatchProcessor.FunctionLibrary.FindProperty.ExactMatch",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchFindProperty_ExactMatch::RunTest(const FString& Parameters)
{
	UBatchTestObject* Obj = NewObject<UBatchTestObject>();
	Obj->AddToRoot();

	const FBatchVariable Variable(Obj);
	FBatchProperty Prop;

	TestTrue(TEXT("精确名称 'bFlag' 应能找到"), UBatchFunctionLibrary::FindProperty(TEXT("bFlag"), Variable, Prop));

	Prop = FBatchProperty();
	TestFalse(TEXT("前缀 'bFla' 不应命中（精确匹配）"), UBatchFunctionLibrary::FindProperty(TEXT("bFla"), Variable, Prop));

	Prop = FBatchProperty();
	TestFalse(TEXT("空名称不应命中"), UBatchFunctionLibrary::FindProperty(TEXT(""), Variable, Prop));

	Prop = FBatchProperty();
	TestFalse(TEXT("不存在的属性名不应命中"), UBatchFunctionLibrary::FindProperty(TEXT("nonexistent_xyz"), Variable, Prop));

	Obj->RemoveFromRoot();
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// FindProperty — 嵌套路径
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchFindProperty_NestedPath,
	"BatchProcessor.FunctionLibrary.FindProperty.NestedPath",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchFindProperty_NestedPath::RunTest(const FString& Parameters)
{
	UBatchTestObject* Obj = NewObject<UBatchTestObject>();
	Obj->AddToRoot();

	const FBatchVariable Variable(Obj);
	FBatchProperty Prop;

	TestTrue(TEXT("嵌套路径 'Nested.bNestedFlag' 应能找到"),
		UBatchFunctionLibrary::FindProperty(TEXT("Nested.bNestedFlag"), Variable, Prop));

	Prop = FBatchProperty();
	TestFalse(TEXT("不存在的嵌套路径不应命中"),
		UBatchFunctionLibrary::FindProperty(TEXT("Nested.nonexistent"), Variable, Prop));

	Obj->RemoveFromRoot();
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// SetProperty — Bool
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchSetProperty_Bool,
	"BatchProcessor.FunctionLibrary.SetProperty.Bool",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchSetProperty_Bool::RunTest(const FString& Parameters)
{
	UBatchTestObject* Obj = NewObject<UBatchTestObject>();
	Obj->AddToRoot();
	Obj->bFlag = false;

	const FBatchVariable Variable(Obj);

	// 相同值 → Same
	TestEqual(TEXT("值相同应返回 Same"),
		UBatchFunctionLibrary::SetProperty(TEXT("bFlag"), Variable, false),
		EBatchSetPropertyResult::Same);

	// 不同值 → Success + 值已写入
	TestEqual(TEXT("不同值应返回 Success"),
		UBatchFunctionLibrary::SetProperty(TEXT("bFlag"), Variable, true),
		EBatchSetPropertyResult::Success);
	TestTrue(TEXT("bFlag 应已被改写为 true"), Obj->bFlag);

	// 类型不匹配（int64 写 bool 属性）→ Failed
	TestEqual(TEXT("类型不匹配应返回 Failed"),
		UBatchFunctionLibrary::SetProperty(TEXT("bFlag"), Variable, int64(1)),
		EBatchSetPropertyResult::Failed);

	// 属性不存在 → NotFound
	TestEqual(TEXT("属性不存在应返回 NotFound"),
		UBatchFunctionLibrary::SetProperty(TEXT("nonexistent_xyz"), Variable, true),
		EBatchSetPropertyResult::NotFound);

	Obj->RemoveFromRoot();
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// SetProperty — Int
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchSetProperty_Int,
	"BatchProcessor.FunctionLibrary.SetProperty.Int",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchSetProperty_Int::RunTest(const FString& Parameters)
{
	UBatchTestObject* Obj = NewObject<UBatchTestObject>();
	Obj->AddToRoot();
	Obj->Count = 10;

	const FBatchVariable Variable(Obj);

	TestEqual(TEXT("值相同应返回 Same"),
		UBatchFunctionLibrary::SetProperty(TEXT("Count"), Variable, int64(10)),
		EBatchSetPropertyResult::Same);

	TestEqual(TEXT("不同值应返回 Success"),
		UBatchFunctionLibrary::SetProperty(TEXT("Count"), Variable, int64(42)),
		EBatchSetPropertyResult::Success);

	TestEqual(TEXT("Count 应已被改写为 42"), Obj->Count, 42);

	// 类型不匹配（bool 写 int32 属性）→ Failed
	TestEqual(TEXT("类型不匹配应返回 Failed"),
		UBatchFunctionLibrary::SetProperty(TEXT("Count"), Variable, true),
		EBatchSetPropertyResult::Failed);

	Obj->RemoveFromRoot();
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// SetProperty — String
// ─────────────────────────────────────────────────────────────────────────────

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBatchSetProperty_String,
	"BatchProcessor.FunctionLibrary.SetProperty.String",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBatchSetProperty_String::RunTest(const FString& Parameters)
{
	UBatchTestObject* Obj = NewObject<UBatchTestObject>();
	Obj->AddToRoot();
	Obj->Name = TEXT("hello");

	const FBatchVariable Variable(Obj);

	TestEqual(TEXT("相同字符串应返回 Same"),
		UBatchFunctionLibrary::SetProperty(TEXT("Name"), Variable, FString(TEXT("hello"))),
		EBatchSetPropertyResult::Same);

	TestEqual(TEXT("不同字符串应返回 Success"),
		UBatchFunctionLibrary::SetProperty(TEXT("Name"), Variable, FString(TEXT("world"))),
		EBatchSetPropertyResult::Success);

	TestEqual(TEXT("Name 应已被改写为 'world'"), Obj->Name, FString(TEXT("world")));

	Obj->RemoveFromRoot();
	return true;
}
