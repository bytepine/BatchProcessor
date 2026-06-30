// Copyright Byteyang Games, Inc. All Rights Reserved.

#pragma once

// ── BatchProcessor 跨版本编译兼容宏 ──────────────────────────────────────────
//
// 将 ENGINE_MAJOR_VERSION / ENGINE_MINOR_VERSION 数值化，使版本比较只需一行。
//
// 使用规则（与 NexusLink NexusVersionCompat.h 保持一致）：
//   - 业务代码只写 #if BP_UE_HAS_<语义>
//   - 禁止在本文件外写 BP_UE_AT_LEAST(...)
//   - 每条宏须对应一个真实的 API 变更点，并注明版本与来源
//
// 新增宏时同步更新 docs/VersionCompatReference.md（若存在）。
// ─────────────────────────────────────────────────────────────────────────────

#ifndef ENGINE_MAJOR_VERSION
#include "Runtime/Launch/Resources/Version.h"
#endif

#define BP_UE_VERSION  (ENGINE_MAJOR_VERSION * 100 + ENGINE_MINOR_VERSION)

#define BP_UE_AT_LEAST(Major, Minor)  (BP_UE_VERSION >= (Major) * 100 + (Minor))

// ── 通用 Editor API 变更点 ────────────────────────────────────────────────────

// UE5.0：TObjectPtr<T> 替代裸指针作为 UPROPERTY 成员（UE4 使用 T*）
#define BP_UE_HAS_OBJECT_PTR          BP_UE_AT_LEAST(5, 0)

// UE5.3：FMessageDialog::Open 的 Title 参数由 const FText* 改为 const FText&（按值）
#define BP_UE_HAS_MESSAGE_DIALOG_TITLE_BY_VALUE  BP_UE_AT_LEAST(5, 3)

// UE5.0：FEditorStyle::Get() → FAppStyle::Get()（头文件 Styling/AppStyle.h）
#define BP_UE_HAS_APP_STYLE           BP_UE_AT_LEAST(5, 0)

// UE5.0：FTicker → FTSTicker（Containers/Ticker.h）
#define BP_UE_HAS_FTSTICKER           BP_UE_AT_LEAST(5, 0)

// UE5.0：MarkPendingKill() → MarkAsGarbage()
#define BP_UE_HAS_MARK_AS_GARBAGE     BP_UE_AT_LEAST(5, 0)

// UE5.0：UPackage::SavePackage 新签名接受 FSavePackageArgs（原 FObjectSaveContext）
#define BP_UE_HAS_SAVE_PACKAGE_ARGS   BP_UE_AT_LEAST(5, 0)

// UE5.0：UTexture2D::PlatformData 改为 GetPlatformData()（返回指针，原为成员）
#define BP_UE_HAS_TEXTURE_PLATFORM_ACCESSOR  BP_UE_AT_LEAST(5, 0)

// UE5.1：StaticFindObject / ANY_PACKAGE → FindFirstObject
#define BP_UE_HAS_FIND_FIRST_OBJECT   BP_UE_AT_LEAST(5, 1)

// UE5.1：FARFilter::ClassNames → ClassPaths（AssetRegistry 过滤器）
#define BP_UE_HAS_CLASS_PATHS         BP_UE_AT_LEAST(5, 1)

// UE5.1：FAssetData::ObjectPath → GetSoftObjectPath()（返回 FSoftObjectPath）
#define BP_UE_HAS_ASSET_SOFT_OBJECT_PATH  BP_UE_AT_LEAST(5, 1)

// UE5.1：FAssetData::AssetClass(FName) → AssetClassPath(FTopLevelAssetPath)；
//        FAssetData::ObjectPath.ToString() → GetObjectPathString()
#define BP_UE_HAS_ASSET_CLASS_PATH_FIELD  BP_UE_AT_LEAST(5, 1)

// UE5.1：FProperty::ImportText → ImportText_Direct；ExportTextItem → ExportTextItem_Direct
#define BP_UE_HAS_IMPORT_TEXT_DIRECT  BP_UE_AT_LEAST(5, 1)
#define BP_UE_HAS_EXPORT_TEXT_DIRECT  BP_UE_AT_LEAST(5, 1)

// UE5.1："EditorStyle" 模块已重命名为 "Styling"（Build.cs 依赖名变更）
#define BP_UE_HAS_STYLING_MODULE      BP_UE_AT_LEAST(5, 1)

// ── Blueprint / Kismet 相关 ───────────────────────────────────────────────────

// UE5.0：UBlueprint 默认值传播推荐流程变更：
//   FBlueprintEditorUtils::MarkBlueprintAsModified + FKismetEditorUtilities::CompileBlueprint
//   在 4.x 中已存在，但 5.0 后 CDO 变更不再隐式触发重编，需显式调用。
//   本宏用于标记代码路径中需要显式调用 CompileBlueprint 的分支。
#define BP_UE_HAS_EXPLICIT_BLUEPRINT_COMPILE  BP_UE_AT_LEAST(5, 0)

// ── 资产保存 / Package 相关 ───────────────────────────────────────────────────

// UE5.3：UPackage::MarkPackageDirty() 在某些定制引擎中标记为 deprecated，
//   推荐改用 MarkDirtyForCooker()；标准引擎中 MarkPackageDirty() 仍可用。
//   BatchAssetSaver 已不依赖此 API，此宏保留供将来扩展。
#define BP_UE_HAS_MARK_DIRTY_FOR_COOKER  BP_UE_AT_LEAST(5, 3)

// ── 容器 / 算法 ───────────────────────────────────────────────────────────────

// UE5.4：EAllowShrinking 枚举引入，TArray::RemoveAt/Pop 等接受 EAllowShrinking 替代 bool
#define BP_UE_HAS_ALLOW_SHRINKING_ENUM  BP_UE_AT_LEAST(5, 4)

#if BP_UE_HAS_ALLOW_SHRINKING_ENUM
    #define BP_ALLOW_SHRINKING_NO   EAllowShrinking::No
    #define BP_ALLOW_SHRINKING_YES  EAllowShrinking::Yes
#else
    #define BP_ALLOW_SHRINKING_NO   false
    #define BP_ALLOW_SHRINKING_YES  true
#endif

// ── 材质相关（BatchProcessor 材质修改器） ─────────────────────────────────────

// UE5.3：MD_Surface 枚举值移至 EMaterialDomain::Surface（需 #include MaterialDomain.h）
#define BP_UE_HAS_SCOPED_MATERIAL_DOMAIN   BP_UE_AT_LEAST(5, 3)

// UE5.3：EMaterialDomain 头文件独立为 MaterialDomain.h（UE4/5.0~5.2 由 Material.h 提供）
#define BP_UE_HAS_MATERIAL_DOMAIN_HEADER   BP_UE_AT_LEAST(5, 3)

// UE5.1：UMaterial::Expressions（TArray<UMaterialExpression*>）
//        → GetEditorOnlyData()->Expressions（编辑器专用数据分离）
#define BP_UE_HAS_MATERIAL_EDITOR_ONLY_DATA  BP_UE_AT_LEAST(5, 1)

// ── 内部版本号辅助（用于需要精确区间判断的情况）────────────────────────────────
//   例：5.0~5.5 之间的特定 API；写法：(BP_UE_VERSION >= 500 && BP_UE_VERSION < 506)
//   无直接宏，业务代码可在单个 .cpp 文件中用 BP_UE_VERSION 内联判断（仅限 .cpp，不在 .h 中）
