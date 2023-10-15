// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SProject : ModuleRules
{
	public SProject(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UMG",
				"GameFeatures",
				"ModularGameplay",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"AIModule",
				"DataValidation",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[] {
				"Slate",
				"SlateCore",
				"UnrealEd",
				"EnhancedInput",
				"CommonUI",
				"CommonInput",
				"GameplayMessageNodes",
			}
		);

		// Plugins
		PrivateDependencyModuleNames.AddRange(
			new[] {
				"AsyncMixin",
				"CommonGame",
				"CommonLoadingScreen",
				"CommonUser",
				"GameplayMessageRuntime",
				"GameSettings",
				"GameSubtitles",
				"ModularGameplayActors",
				"PocketWorlds",
				"UIExtension",
			}
		);


PublicIncludePaths.AddRange(
			new[]
			{
				ModuleDirectory,
			}
		);

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
