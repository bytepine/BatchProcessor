// Copyright Byteyang Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProcessorTests : ModuleRules
{
    public BatchProcessorTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 定制引擎（如 LetsGo）WITH_EDITOR_ENCRYPTION 兜底（与 BatchProcessor.Build.cs 同步）
        ApplyCompatDefines(this);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "BatchProcessor",
        });

        // 测试模块仅 Editor target 编译；避免 BuildPlugin Game 阶段拉入 UnrealEd
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");

            PrivateIncludePaths.AddRange(new string[]
            {
                "BatchProcessor/Private",
                "BatchProcessor/Public",
            });
        }
    }

    // ── WITH_EDITOR_ENCRYPTION 兜底（与 BatchProcessor.Build.cs 同步维护） ──────

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
