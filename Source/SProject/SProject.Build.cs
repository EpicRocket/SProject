// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SProject : ModuleRules
{
	public SProject(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "SProject.h";

		PublicDependencyModuleNames.AddRange(
			new[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"SlateCore",
				"UMG",
				"AIModule",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[] {
				"GameFeatures",
				"Slate",
				"UnrealEd",
				"EnhancedInput",
				"CommonUI",
				"CommonInput",
				"ModularGameplay",
				"DataValidation",
                "DeveloperSettings",
            }
		);

		// GameplayAbility
		PrivateDependencyModuleNames.AddRange(
			new[] {
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
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
