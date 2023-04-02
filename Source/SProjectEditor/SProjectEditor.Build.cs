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

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
