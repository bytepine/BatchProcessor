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
            "ContentBrowser",
        });

        // AssetDefinition module was introduced in UE 5.2
        bool bHasAssetDefinition = Target.Version.MajorVersion > 5 ||
                                   (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 2);
        if (bHasAssetDefinition)
        {
            PrivateDependencyModuleNames.Add("AssetDefinition");
        }
    }
}
