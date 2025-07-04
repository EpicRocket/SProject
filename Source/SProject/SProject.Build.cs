

using UnrealBuildTool;
using System.IO;

public class SProject : ModuleRules
{
	public SProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "SProject.h";

		PublicIncludePaths.AddRange(
			new[] {
				ModuleDirectory,
			}
		);

		PublicDependencyModuleNames.AddRange(
			new[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"SlateCore",
				"UMG",
				"AIModule",
				"NavigationSystem",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[] {
				"Slate",
				"EnhancedInput",
				"DeveloperSettings",
				"AssetRegistry",
				"ApplicationCore",
			}
		);

		// For Editor
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new[] {
				"DataValidation",
				"UnrealEd",
				}
			);
		}

		// Common UI
		PrivateDependencyModuleNames.AddRange(
			new[] {
				"CommonUI",
				"CommonInput",
			}
		);

		// GameplayAbility
		PrivateDependencyModuleNames.AddRange(
			new[] {
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
			}
		);

		// Plugins ETC
		PrivateDependencyModuleNames.AddRange(
			new[] {
				"GGameCore",
				"ModularGameplay"
			}
		);

		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");

		SetupGameplayDebuggerSupport(Target);

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
