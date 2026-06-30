// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

// ── BatchProcessorEditor 跨版本 AppStyle 兼容 ──────────────────────────────
//
// UE 5.0 引入 FAppStyle（Styling/AppStyle.h）替代 FEditorStyle。
// UE 4.x 使用 FEditorStyle（EditorStyleSet.h）。
// 统一用 BATCH_APP_STYLE 代替直接写 FAppStyle，保证 4.x / 5.x 均可编译。
//
// 使用方式：
//   #include "BatchEditorStyleCompat.h"
//   BATCH_APP_STYLE::GetAppStyleSetName()
//   BATCH_APP_STYLE::Get().GetBrush(...)
// ─────────────────────────────────────────────────────────────────────────────

#include "Utils/BatchVersionCompat.h"

#if BP_UE_HAS_APP_STYLE
    #include "Styling/AppStyle.h"
    #define BATCH_APP_STYLE           FAppStyle
    #define BATCH_APP_STYLE_SET_NAME  FAppStyle::GetAppStyleSetName()
#else
    #include "EditorStyleSet.h"
    #define BATCH_APP_STYLE           FEditorStyle
    #define BATCH_APP_STYLE_SET_NAME  FEditorStyle::GetStyleSetName()
#endif
