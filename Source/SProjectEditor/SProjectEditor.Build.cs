// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SProjectEditor : ModuleRules
{
	public SProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new [] 
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"UnrealEd",
				"Slate",
				"SlateCore",
				"UMG",
				"Blutility",
				"UMGEditor",
				"PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"BlueprintGraph",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"SProject",
			}
		);

		PublicIncludePaths.AddRange(
			new[] {
				ModuleDirectory,
			}
		);

        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDependencyModuleNames.Add("OpenXLSX");
		}

	}
}
