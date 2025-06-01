
using UnrealBuildTool;

public class GGameCoreEditor : ModuleRules
{
	public GGameCoreEditor(ReadOnlyTargetRules Target) : base(Target)
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
		new string[]
			{
				"BlueprintGraph",
				"KismetCompiler",
				"PropertyEditor",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"UMG",
				"Blutility",
				"UMGEditor",
				"ToolMenus",
				"AssetTools",
				"DeveloperSettings",
				"EditorSubsystem",
				"ContentBrowser",
			}
		);

		// Internal Module
		PrivateDependencyModuleNames.AddRange(
		new string[]
			{
				"GGameCore",
			}
		);

		// External Plugins
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"OpenXLSX",
			}
		);
	}
}
