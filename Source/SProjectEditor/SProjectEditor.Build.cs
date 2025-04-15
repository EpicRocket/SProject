// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SProjectEditor : ModuleRules
{
	public SProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new[] {
				ModuleDirectory,
			}
		);

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Json",
				"JsonUtilities",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
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
				"DeveloperSettings",
				"EditorSubsystem",
				"ContentBrowser",
			}
		);

		// 내부 모듈
		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"SProject",
			}
		);

		// 외부 플러그인
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"OpenXLSX",
			}
		);
	}
}
