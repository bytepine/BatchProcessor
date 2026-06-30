// Copyright Byteyang Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProcessor : ModuleRules
{
    public BatchProcessor(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // 定制引擎（如 LetsGo）WITH_EDITOR_ENCRYPTION 兜底
        ApplyCompatDefines(this);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
        });

        // Editor-only 依赖：Game target 构建时不得引用 UnrealEd（BuildPlugin 会同时编两套 target）
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Slate",
                "SlateCore",
                "ToolMenus",
                "UnrealEd",
                "EditorStyle",
            });
        }
    }

    // ── WITH_EDITOR_ENCRYPTION 兜底（与 NexusLink.Build.cs 同步维护） ──────────
    // 定制引擎在 Misc/Build.h 中可能声明 WITH_EDITOR_ENCRYPTION；
    // UBT 将其视为未定义符号导致编译失败，此处提前注入兜底定义 0。

    private static void ApplyCompatDefines(ModuleRules Module)
    {
        if (ShouldDefineWithEditorEncryption(Module))
        {
            Module.PublicDefinitions.Add("WITH_EDITOR_ENCRYPTION=0");
        }
    }

    private static bool ShouldDefineWithEditorEncryption(ModuleRules Module)
    {
        foreach (string engineDir in ResolveEngineDirectoryCandidates(Module))
        {
            string buildH = System.IO.Path.Combine(
                engineDir, "Source", "Runtime", "Core", "Public", "Misc", "Build.h");
            if (!System.IO.File.Exists(buildH)) continue;
            try
            {
                if (System.IO.File.ReadAllText(buildH).Contains("WITH_EDITOR_ENCRYPTION"))
                    return true;
            }
            catch (System.Exception) { }
        }
        return System.Environment.GetEnvironmentVariable(
            "BATCH_WITH_EDITOR_ENCRYPTION_FALLBACK") == "1";
    }

    private static System.Collections.Generic.List<string> ResolveEngineDirectoryCandidates(
        ModuleRules Module)
    {
        var seen = new System.Collections.Generic.HashSet<string>(
            System.StringComparer.OrdinalIgnoreCase);
        var list = new System.Collections.Generic.List<string>();

        // EngineDirectory 在 UE 5.8 改为 static 属性；用反射兼容 UE4~5.7 和 5.8+
        try
        {
            var Prop = typeof(ModuleRules).GetProperty("EngineDirectory",
                System.Reflection.BindingFlags.Public |
                System.Reflection.BindingFlags.Instance |
                System.Reflection.BindingFlags.Static);
            if (Prop != null)
            {
                bool IsStatic = Prop.GetGetMethod().IsStatic;
                string EngDir = IsStatic
                    ? Prop.GetValue(null) as string
                    : Prop.GetValue(Module) as string;
                TryAddEngineDirectory(EngDir, seen, list);
            }
        }
        catch { }

        TryAddEngineDirectory(
            System.Environment.GetEnvironmentVariable("UE_ENGINE_DIRECTORY"), seen, list);
        TryAddEngineDirectory(
            System.Environment.GetEnvironmentVariable("UE4_ROOT"), seen, list);
        TryAddEngineDirectory(
            System.Environment.GetEnvironmentVariable("UNREAL_ENGINE_PATH"), seen, list);

        return list;
    }

    private static void TryAddEngineDirectory(
        string dir,
        System.Collections.Generic.HashSet<string> seen,
        System.Collections.Generic.List<string> list)
    {
        if (string.IsNullOrEmpty(dir)) return;
        try
        {
            dir = System.IO.Path.GetFullPath(dir);
            if (System.IO.Directory.Exists(dir) && seen.Add(dir))
                list.Add(dir);
        }
        catch (System.Exception) { }
    }
}
