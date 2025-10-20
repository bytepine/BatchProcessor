using UnrealBuildTool;

public class BatchProcessor : ModuleRules
{
    public BatchProcessor(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "EditorStyle",
                "ToolMenus",
                "UnrealEd",
            }
        );
    }
}