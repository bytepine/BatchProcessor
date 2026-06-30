// Copyright Byteyang Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProcessorEditor : ModuleRules
{
    public BatchProcessorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "BatchProcessor",
            "AssetTools",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "InputCore",
            "UnrealEd",
            "EditorStyle",
            "PropertyEditor",
            "ToolMenus",
            "AssetDefinition",
            "ContentBrowser",
        });
    }
}
