// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

// UE 5.7+ 提供自由常量 EAutomationTestFlags_ApplicationContextMask；
// 更早版本须手动组合 EditorContext | ProductFilter。
#if defined(EAutomationTestFlags_ApplicationContextMask)
	#define BATCH_AUTOMATION_TEST_FLAGS (EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
#else
	#define BATCH_AUTOMATION_TEST_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
#endif
